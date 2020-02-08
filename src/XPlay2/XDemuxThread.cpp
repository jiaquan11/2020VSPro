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
	//����Ƶ�������ʹ����߳�
	if (!vt->Open(demux->CopyVPara(), call, demux->width, demux->height)) {
		ret = false;
		cout << "vt->Open failed" << endl;
	}
	//����Ƶ�������ʹ����߳�
	if (!at->Open(demux->CopyAPara(), demux->sampleRate, demux->channels)) {
		ret = false;
		cout << "at->Open failed" << endl;
	}

	mux.unlock();
	cout << "XDemuxThread::Open ret: " << ret << endl;
	return ret;
}

//���������߳�
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

	//������ǰ�߳�
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

		//����Ƶͬ��
		if (vt && at) {
			vt->synpts = at->pts;
		}

		AVPacket *pkt = demux->Read();
		if (!pkt) {
			mux.unlock();
			msleep(5);
			continue;
		}
		//�ж���������Ƶ
		if (demux->IsAudio(pkt)) {//��Ƶ
			if (at) {
				at->Push(pkt);
			}
		}else {//��Ƶ
			if (vt) {
				vt->Push(pkt);
			}
		}

		mux.unlock();

		msleep(1);//�ӳ�1���룬�����ȡ̫�죬��������Ƶͬ������ʱ
	}
}