#include "XDemux.h"
#include <iostream>
using namespace std;

extern "C" {
#include "libavformat/avformat.h"
}

#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")

static double r2d(AVRational r) {
	return r.den == 0 ? 0 : (double)r.num / (double)r.den;
}

void XSleep(int ms) {
	//c++ 11
	chrono::milliseconds du(ms);
	this_thread::sleep_for(du);
}

XDemux::XDemux(){
	static bool isFirst = true;
	static std::mutex dmux;
	
	dmux.lock();
	if (isFirst) {
		//��ʼ����װ��
		av_register_all();

		//��ʼ�������,(���Դ�rtsp, rtmp, httpЭ�����ý����Ƶ)
		avformat_network_init();
		isFirst = false;
	}
	dmux.unlock();
	
}

XDemux::~XDemux(){

}

bool XDemux::Open(const char* url) {
	AVDictionary *opts = NULL;
	//����rtsp����tcpЭ���
	av_dict_set(&opts, "rtsp_transport", "tcp", 0);
	//������ʱʱ��
	av_dict_set(&opts, "max_delay", "500", 0);

	mux.lock();
	int ret = avformat_open_input(
		&ic,
		url,
		NULL, //NULL��ʾ�Զ�ѡ������
		&opts);//�������ã�����rtsp����ʱʱ��
	if (ret != 0) {
		mux.unlock();

		char buf[1024] = { 0 };
		av_strerror(ret, buf, sizeof(buf) - 1);
		cout << "open " << url << " failed: " << buf << endl;
		return false;
	}
	cout << "open " << url << " success!" << endl;

	//��ȡ����Ϣ
	ret = avformat_find_stream_info(ic, 0);
	//��ʱ��������
	totalMs = ic->duration / (AV_TIME_BASE / 1000);
	cout << "totalMs: " << totalMs << endl;

	//��ӡ��Ƶ������ϸ��Ϣ
	av_dump_format(ic, 0, url, 0);

	//��ȡ��Ƶ��
	videoStream = av_find_best_stream(ic, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
	cout << "==========================================" << endl;
	cout << videoStream << " ��Ƶ��Ϣ" << endl;
	AVStream* as = ic->streams[videoStream];
	cout << "codec_id = " << as->codecpar->codec_id << endl;
	cout << "format = " << as->codecpar->format << endl;
	
	cout << "width: " << as->codecpar->width << endl;
	cout << "height: " << as->codecpar->height << endl;
	//֡�� fps ����ת��
	cout << "video fps = " << r2d(as->avg_frame_rate) << endl;
	cout << "video AVStream num: " << as->time_base.num << " den: " << as->time_base.den << endl;

	//��ȡ��Ƶ��
	audioStream = av_find_best_stream(ic, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
	cout << "==========================================" << endl;
	cout << audioStream << " ��Ƶ��Ϣ" << endl;
	as = ic->streams[audioStream];
	cout << "codec_id = " << as->codecpar->codec_id << endl;
	cout << "format = " << as->codecpar->format << endl;
	cout << "sample_rate = " << as->codecpar->sample_rate << endl;
	//AVSampleFormat
	cout << "channels = " << as->codecpar->channels << endl;
	//֡�� fps ����ת��
	cout << "audio fps = " << r2d(as->avg_frame_rate) << endl;;
	cout << "audio AVStream num: " << as->time_base.num << " den: " << as->time_base.den << endl;

	//һ֡����??��ͨ��������
	cout << "frame_size: " << as->codecpar->frame_size << endl;

	mux.unlock();

	return true;
}
