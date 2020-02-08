#include "XDecode.h"
#include <iostream>
using namespace std;

extern "C" {
#include <libavcodec/avcodec.h>
}

XDecode::XDecode(){

}

XDecode::~XDecode(){

}

void XFreePacket(AVPacket** pkt) {
	if (pkt || *pkt) return;

	av_packet_free(pkt);
}

//打开解码器
bool XDecode::Open(AVCodecParameters *para) {
	if (!para) 
		return false;
	
	//打开之前先关闭解码器资源
	Close();

	//找到解码器
	AVCodec *codec = avcodec_find_decoder(para->codec_id);
	if (!codec) {
		avcodec_parameters_free(&para);
		cout << "can't find the codec id" << para->codec_id << endl;
		return false;
	}
	cout << "find the AVCodec " << para->codec_id << endl;

	mux.lock();
	//解码器打开
	//创建解码器上下文
	avc = avcodec_alloc_context3(codec);
	//配置解码器上下文参数
	avcodec_parameters_to_context(avc, para);
	avcodec_parameters_free(&para);

	//八线程解码
	avc->thread_count = 8;
	//打开解码器上下文
	int ret = avcodec_open2(avc, 0, 0);
	if (ret != 0) {
		avcodec_free_context(&avc);
		mux.unlock();
		char buf[1024] = { 0 };
		av_strerror(ret, buf, sizeof(buf) - 1);
		cout << "avcodec_open2 failed: " << buf << endl;
		return false;
	}
	mux.unlock();
	cout << "avcodec_open2 success!" << endl;
	return true;
}

void XDecode::Clear() {
	mux.lock();
	//清理解码缓冲
	if (avc) {
		avcodec_flush_buffers(avc);
	}
	mux.unlock();
}

void XDecode::Close() {
	mux.lock();
	if (avc) {
		avcodec_close(avc);
		avcodec_free_context(&avc);
	}
	pts = 0;
	mux.unlock();
}

//发送到解码线程，不管成功与否都释放pkt空间(对象和媒体内容)
bool XDecode::Send(AVPacket* pkt) {
	//容错处理
	if (!pkt || (pkt->size <= 0) || !pkt->data) {
		return false;
	}

	mux.lock();
	if (!avc) {
		mux.unlock();
		return false;
	}
	int ret = avcodec_send_packet(avc, pkt);
	mux.unlock();
	av_packet_free(&pkt);

	if (ret != 0){
		return false;
	}

	return true;
}

//获取解码数据，一次send可能需要多次Recv,获取缓冲中的数据Send NULL 再多次Recv
//每次复制一份，由调用者释放av_frame_free
AVFrame* XDecode::Recv() {
	mux.lock();
	if (!avc) {
		mux.unlock();
		return NULL;
	}
	AVFrame* frame = av_frame_alloc();
	int ret = avcodec_receive_frame(avc, frame);
	mux.unlock();
	if (ret != 0) {
		av_frame_free(&frame);
		return NULL;
	}
	//cout << "["<<frame->linesize[0] << "] " << flush;
	pts = frame->pts;//解码时间戳
	return frame;
}
