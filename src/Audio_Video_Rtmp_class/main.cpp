#include <QtCore/QCoreApplication>
#include "XMediaEncode.h"
#include "XRtmp.h"
#include "XAudioRecord.h"
#include "XVideoCapture.h"

#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	//nginx-rtmp 直播服务器推流url
	char* outurl = "rtmp://192.168.174.129/live/";
	int ret = 0;

	int sampleRate = 44100;
	int channels = 2;
	int sampleByte = 2;
	int nbSamples = 1024;

	//打开摄像机
	XVideoCapture* xv = XVideoCapture::Get();
	if (!xv->Init(0)) {
		cout << "open camera failed" << endl;
		getchar();
		return -1;
	}
	cout << "open camera success!" << endl;

	//开启视频录制线程
	xv->Start();

	//1 qt音频开始录制
	XAudioRecord* ar = XAudioRecord::Get();
	ar->sampleRate = sampleRate;
	ar->channels = channels;
	ar->sampleByte = sampleByte;
	ar->nbSamples = nbSamples;
	if (!ar->Init()) {
		cout << "XAudioRecord Init failed" << endl;
		getchar();
		return -1;
	}

	//开启音频录制线程
	ar->Start();

	//音视频编码类
	XMediaEncode* xe = XMediaEncode::Get();

	//初始化格式转换上下文
	//初始化输出的数据结构
	xe->inWidth = xv->width;
	xe->inHeight = xv->height;
	xe->outWidth = xv->width;
	xe->outHeight = xv->height;
	xe->fps = xv->fps;
	if (!xe->InitScale()) {
		getchar();
		return -1;
	}
	cout << "初始化视频转换上下文成功！" << endl;

	xe->channels = channels;
	xe->nbSample = nbSamples;
	xe->sampleRate = sampleRate;
	xe->inSampleFmt = XSampleFMT::X_S16;
	xe->outSampleFmt = XSampleFMT::X_FLTP;
	if (!xe->InitResample()) {
		getchar();
		return -1;
	}

	//4 初始化音频编码器
	if (!xe->InitAudioCodec()) {
		getchar();
		return -1;
	}

	// 初始化视频编码器
	if (!xe->InitVideoCodec()) {
		getchar();
		return -1;
	}

	//5 输出封装器和音频流配置
	//a 创建输出封装器上下文
	XRtmp* xr = XRtmp::Get(0);
	if (!xr->Init(outurl)) {
		getchar();
		return -1;
	}

	//b 添加视频流
	int vindex = xr->AddStream(xe->vc);
	if (vindex < 0) {
		getchar();
		return -1;
	}
		
	//b 添加音频流
	int aindex = xr->AddStream(xe->ac);
	if (aindex < 0) {
		getchar();
		return -1;
	}

	//打开rtmp的网络输出IO
	//写入封装头
	if (!xr->SendHead()) {
		getchar();
		return -1;
	}

	ar->Clear();
	xv->Clear();
	long long beginTime = GetCurTime();//基准计时开始点

	/*
	视频录制推流音视频同步逻辑:
	1.同时开启音频采集线程和视频采集线程，并获取当前采集到数据的时间点
	2.设置一个基准计时开始点
	3.音视频各自将获取数据的时间点减去时间基准点的差值，作为推流的时间戳
	4.如此可以较精准地保证推流同步
	*/
	for (;;) {
		//一次读取一帧音频
		XData ad = ar->Pop();
		XData vd = xv->Pop();
		if ((ad.size <= 0) && (vd.size <= 0)) {
			QThread::msleep(1);
			continue;
		}

		//处理音频
		if (ad.size > 0) {
			ad.pts = ad.pts - beginTime;

			//重采样源数据
			XData pcm = xe->Resample(ad);
			ad.Drop();

			XData pkt = xe->EncodeAudio(pcm);
			if (pkt.size > 0) {
				//推流
				if (xr->SendFrame(pkt, aindex)) {
					cout << "#" << flush;
				}
			}
		}

		//处理视频
		if (vd.size > 0) {
			vd.pts = vd.pts - beginTime;

			XData yuv = xe->RGBToYUV(vd);
			vd.Drop();

			XData pkt = xe->EncodeVideo(yuv);
			cout << " V:"<<pkt.size << flush;
			if (pkt.size > 0) {
				//推流
				if (xr->SendFrame(pkt, vindex)) {
					cout << "@" << flush;
				}
			}
		}
	}

	getchar();

	return a.exec();
}
