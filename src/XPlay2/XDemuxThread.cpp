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

//�ر��̣߳�������Դ
void XDemuxThread::Close() {
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

void XDemuxThread::Seek(double pos) {
	//������
	Clear();

	mux.lock();
	bool status = this->isPause;
	mux.unlock();

	//��ͣ
	SetPause(true);

	mux.lock();
	if (demux) {
		demux->Seek(pos);
	}
	//ʵ��Ҫ��ʾ��λ��pts
	long long seekPts = pos * demux->totalMs;
	while (!isExit) {
		AVPacket* pkt = demux->ReadVideo();
		if (!pkt) break;
		//������뵽seekPts
		if (vt->RepaintPts(pkt, seekPts)) {
			this->pts = seekPts;
			break;
		}
	}

	mux.unlock();

	//seek�Ƿ���ͣ״̬
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

		//����Ƶͬ��
		if (vt && at) {
			pts = at->pts;//���︳ֵ�����ڽ�������ʾ��
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