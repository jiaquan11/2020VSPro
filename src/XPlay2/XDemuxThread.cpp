#include "XDemuxThread.h"
#include "XDemux.h"
#include "XVideoThread.h"
#include "XAudioThread.h"
#include <iostream>
using namespace std;

XDemuxThread::XDemuxThread() {

}


XDemuxThread::~XDemuxThread(){
	isExit = true;
	wait();
}

bool XDemuxThread::Open(const char* url, IVideoCall* call) {
	if ((url == NULL) || (url[0] == '\0')) {
		return false;
	}

	mux.lock();

	if (!demux) {
		demux = new XDemux();
	}
	if (!vt) {
		vt = new XVideoThread();
	}
	if (!at) {
		at = new XAudioThread();
	}

	bool ret = demux->Open(url);
	if (!ret) {
		cout << "demux->Open(url) failed!" << endl;
		return false;
	}
	//打开视频解码器和处理线程
	if (!vt->Open(demux->CopyVPara(), call, demux->width, demux->height)) {
		ret = false;
		cout << "vt->Open failed" << endl;
	}
	//打开视频解码器和处理线程
	if (!at->Open(demux->CopyAPara(), demux->sampleRate, demux->channels)) {
		ret = false;
		cout << "at->Open failed" << endl;
	}

	mux.unlock();
	cout << "XDemuxThread::Open ret: " << ret << endl;
	return ret;
}

//启动所有线程
void XDemuxThread::Start() {
	mux.lock();
	if (!demux) {
		demux = new XDemux();
	}
	if (!vt) {
		vt = new XVideoThread();
	}
	if (!at) {
		at = new XAudioThread();
	}

	//启动当前线程
	QThread::start();
	if (vt) vt->start();
	if (at) at->start();
	mux.unlock();
}

void XDemuxThread::run() {
	while (!isExit) {
		mux.lock();
		if (!demux) {
			mux.unlock();
			msleep(5);
			continue;
		}

		//音视频同步
		if (vt && at) {
			vt->synpts = at->pts;
		}

		AVPacket *pkt = demux->Read();
		if (!pkt) {
			mux.unlock();
			msleep(5);
			continue;
		}
		//判断数据是音频
		if (demux->IsAudio(pkt)) {//音频
			if (at) {
				at->Push(pkt);
			}
		}else {//视频
			if (vt) {
				vt->Push(pkt);
			}
		}

		mux.unlock();

		msleep(1);//延迟1毫秒，避免读取太快，导致音视频同步不及时
	}
}