#include "XDemuxThread.h"
#include "XDemux.h"
#include "XVideoThread.h"
#include "XAudioThread.h"
#include "XDecode.h"

#include <iostream>
using namespace std;
extern "C" {
#include <libavformat/avformat.h>
}

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
		mux.unlock();
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

	totalMs = demux->totalMs;

	mux.unlock();
	cout << "XDemuxThread::Open ret: " << ret << endl;
	return ret;
}

void XDemuxThread::Clear() {
	mux.lock();
	if (demux) demux->Clear();
	if (vt) vt->Clear();
	if (at) at->Clear();
	mux.unlock();
}

//关闭线程，清理资源
void XDemuxThread::Close() {
	//等待当前线程结束
	isExit = true;
	wait();

	if (vt) vt->Close();
	if (at) at->Close();

	mux.lock();
	delete vt;
	delete at;
	vt = NULL;
	at = NULL;
	mux.unlock();
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

void XDemuxThread::Seek(double pos) {
	//清理缓存
	Clear();

	mux.lock();
	bool status = this->isPause;
	mux.unlock();

	//暂停
	SetPause(true);

	mux.lock();
	if (demux) {
		demux->Seek(pos);
	}
	//实际要显示的位置pts
	long long seekPts = pos * demux->totalMs;
	while (!isExit) {
		AVPacket* pkt = demux->ReadVideo();
		if (!pkt) break;
		//如果解码到seekPts
		if (vt->RepaintPts(pkt, seekPts)) {
			this->pts = seekPts;
			break;
		}
	}

	mux.unlock();

	//seek是非暂停状态
	if (!status) {
		SetPause(false);
	}
}

void XDemuxThread::SetPause(bool isPause) {
	mux.lock();
	this->isPause = isPause;
	if (at) at->SetPause(isPause);
	if (vt) vt->SetPause(isPause);
	mux.unlock();
}

void XDemuxThread::run() {
	while (!isExit) {
		mux.lock();
		if (isPause) {
			mux.unlock();
			msleep(5);
			continue;
		}

		if (!demux) {
			mux.unlock();
			msleep(5);
			continue;
		}

		//音视频同步
		if (vt && at) {
			pts = at->pts;//这里赋值是用于进度条显示的
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