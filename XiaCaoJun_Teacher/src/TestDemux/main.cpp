#include <iostream>
#include <thread>
extern "C" {
#include "libavformat/avformat.h"
}
using namespace std;
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "avcodec.lib")

static double r2d(AVRational r) {
	return r.den == 0 ? 0 : (double)r.num / (double)r.den;
}

void XSleep(int ms) {
	//c++ 11
	chrono::milliseconds du(ms);
	this_thread::sleep_for(du);
}

int main(int argc, char *argv[]) {
	cout << "Test Demux FFmpeg.club" << endl;
	const char *path = "testvideo1.mp4";  

	//初始化封装库
	av_register_all();

	//初始化网络库,(可以打开rtsp, rtmp, http协议的流媒体视频)
	avformat_network_init();

	AVDictionary *opts = NULL;
	//设置rtsp流以tcp协议打开
	av_dict_set(&opts, "rtsp_transport", "tcp", 0);
	//网络延时时间
	av_dict_set(&opts, "max_delay", "500", 0);

	//解封装上下文
	AVFormatContext *ic = NULL;
	int ret = avformat_open_input(
		&ic, 
		path, 
		NULL, //NULL表示自动选择解封器
		&opts);//参数设置，比如rtsp的延时时间
	if (ret != 0) {
		char buf[1024] = { 0 };
		av_strerror(ret, buf, sizeof(buf)-1);
		cout << "open " << path << " failed: " << buf << endl;
		getchar();
		return -1;
	}
	cout << "open " << path << " success!" << endl;

	//获取流信息
	ret = avformat_find_stream_info(ic, 0);
	//总时长，毫秒
	int totalMs = ic->duration / (AV_TIME_BASE / 1000);
	cout << "totalMs: " << totalMs << endl;

	//打印视频流的详细信息
	av_dump_format(ic, 0, path, 0);

	//音视频索引，读取时区分音视频
	int videoStream = 0;
	int audioStream = 1;

	//获取音视频流信息(遍历，函数获取)
	for (int i = 0; i < ic->nb_streams; i++){
		AVStream* as = ic->streams[i];
		//音频AVMEDIA_TYPE_AUDIO
		if (as->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
			audioStream = i;
			cout << i << " 音频信息" << endl;
			cout << "sample_rate = " << as->codecpar->sample_rate << endl;
			//AVSampleFormat
			cout << "format = " << as->codecpar->format << endl;
			cout << "channels = " << as->codecpar->channels << endl;
			cout << "codec_id = " << as->codecpar->codec_id << endl;
			//帧率 fps 分数转换
			cout << "audio fps = " << r2d(as->avg_frame_rate) << endl;;
			cout << "audio AVStream num: " << as->time_base.num << " den: " << as->time_base.den << endl;

			//一帧数据??单通道样本数
			cout << "frame_size: " << as->codecpar->frame_size << endl;
		}else if(as->codecpar->codec_type == AVMEDIA_TYPE_VIDEO){//视频 AVMEDIA_TYPE_VIDEO
			videoStream = i;
			cout << i << " 视频信息" << endl; 
			cout << "width: " << as->codecpar->width << endl;
			cout << "height: " << as->codecpar->height << endl;
			//帧率 fps 分数转换
			cout<<"video fps = "<<r2d(as->avg_frame_rate)<<endl;
			cout << "video AVStream num: " << as->time_base.num<<" den: "<<as->time_base.den<< endl;
		}
	}

	//获取视频流
	videoStream = av_find_best_stream(ic, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);

	AVPacket* pkt = av_packet_alloc();
	for (;;){
		int ret = av_read_frame(ic, pkt);
		if (ret != 0) {
			//循环播放
			cout << "========================end==============================" << endl;
			int ms = 5000;//三秒位置 根据时间基数(分数)转换
			long long pos = (double)ms / (double)1000 / r2d(ic->streams[pkt->stream_index]->time_base);
			cout << "pos: "<<pos << endl;
			
			av_seek_frame(ic, videoStream, pos, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME);
			continue;
		}

		if (pkt->stream_index == videoStream) {
#if 1
			cout << "图像" << endl;
			cout << "pkt->size: " << pkt->size << endl;
			//显示时间
			cout << "pkt->pts: " << pkt->pts << endl;
			//解码时间
			cout << "pkt->dts: " << pkt->dts << endl;

			//转换为毫秒,方便同步
			cout << "pkt->pts ms = " << pkt->pts*(r2d(ic->streams[pkt->stream_index]->time_base) * 1000) << endl;
			//解码时间
			cout << "pkt->dts ms = " << pkt->dts*(r2d(ic->streams[pkt->stream_index]->time_base) * 1000) << endl;
#endif
		}
		if (pkt->stream_index == audioStream) {
#if 0
			//cout << "音频" << endl;
			cout << "pkt->size: " << pkt->size << endl;
			//显示时间
			cout << "pkt->pts: " << pkt->pts << endl;
			//解码时间
			cout << "pkt->dts: " << pkt->dts << endl;

			//转换为毫秒,方便同步
			cout << "pkt->pts ms = " << pkt->pts*(r2d(ic->streams[pkt->stream_index]->time_base) * 1000) << endl;
			//解码时间
			cout << "pkt->dts ms = " << pkt->dts*(r2d(ic->streams[pkt->stream_index]->time_base) * 1000) << endl;
#endif
		}

		//释放，引用计数-1，为0释放空间
		av_packet_unref(pkt);

		//XSleep(500);
	}
	av_packet_free(&pkt);

	if (ic) {
		//释放封装上下文，并且把ic置0
		avformat_close_input(&ic);
	}
	getchar();
	return 0;
}