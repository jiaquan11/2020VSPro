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

//�򿪽�����
bool XDecode::Open(AVCodecParameters *para) {
	if (!para) 
		return false;
	
	//��֮ǰ�ȹرս�������Դ
	Close();

	//�ҵ�������
	AVCodec *codec = avcodec_find_decoder(para->codec_id);
	if (!codec) {
		avcodec_parameters_free(&para);
		cout << "can't find the codec id" << para->codec_id << endl;
		return false;
	}
	cout << "find the AVCodec " << para->codec_id << endl;

	mux.lock();
	//��������
	//����������������
	avc = avcodec_alloc_context3(codec);
	//���ý����������Ĳ���
	avcodec_parameters_to_context(avc, para);
	avcodec_parameters_free(&para);

	//���߳̽���
	avc->thread_count = 8;
	//�򿪽�����������
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
	//������뻺��
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

//���͵������̣߳����ܳɹ�����ͷ�pkt�ռ�(�����ý������)
bool XDecode::Send(AVPacket* pkt) {
	//�ݴ���
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

//��ȡ�������ݣ�һ��send������Ҫ���Recv,��ȡ�����е�����Send NULL �ٶ��Recv
//ÿ�θ���һ�ݣ��ɵ������ͷ�av_frame_free
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
	pts = frame->pts;//����ʱ���
	return frame;
}
