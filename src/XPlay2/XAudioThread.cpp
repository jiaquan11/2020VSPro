#include "XAudioThread.h"
#include "XAudioPlay.h"
#include "XResample.h"
#include "XDecode.h"
#include <iostream>
using namespace std;

XAudioThread::XAudioThread(){
	if (!res) {
		res = new XResample();
	}
	if (!ap) {
		ap = XAudioPlay::Get();
	}
}


XAudioThread::~XAudioThread(){
	//�ȴ��߳��˳�
	isExit = true;
	wait();
}

bool XAudioThread::Open(AVCodecParameters* para, int sampleRate, int channels) {
	if (!para) return false;

	Clear();

	amux.lock();
	pts = 0;

	bool ret = true;
	if (!res->Open(para, false)) {
		cout << "XResample open failed" << endl;
		ret = false;
	}
	ap->sampleRate = sampleRate;
	ap->channels = channels;

	if (!ap->Open()) {
		cout << "XAudioPlay open failed" << endl;
		ret = false;
	}

	if (!decode->Open(para)) {
		cout << "Audio XDecode open failed" << endl;
		ret = false;
	}

	amux.unlock();
	cout << "XAudioThread open ret: " <<ret<< endl;

	return ret; 
}

void XAudioThread::Clear() {

	XDecodeThread::Clear();
	amux.lock();
	if (ap) ap->Clear();
	amux.unlock();
}

//ֹͣ�̣߳�������Դ
void XAudioThread::Close() {

	XDecodeThread::Close();

	if (res) {
		res->Close();
		amux.lock();
		delete res;
		res = NULL;
		amux.unlock();
	}

	if (ap) {
		ap->Close();
		amux.lock();
		ap = NULL;
		amux.unlock();
	}
}

void XAudioThread::SetPause(bool isPause) {
	//amux.lock();
	this->isPause = isPause;
	if (ap) {
		ap->SetPause(isPause);
	}

	//amux.unlock();
}

void XAudioThread::run() {
	unsigned char* pcm = new unsigned char[1024*1024*10];
	while (!isExit) {
		amux.lock();

		if (isPause) {
			amux.unlock();
			msleep(5);
			continue;
		}

		AVPacket* pkt = Pop();
		bool ret = decode->Send(pkt);
		if (!ret) {
			amux.unlock();
			msleep(1);
			continue;
		}
		//һ��send,���recv
		while (!isExit){
			AVFrame* frame = decode->Recv();
			if (!frame) break;

			//��ȥ������δ���ŵ�ʱ��
			pts = decode->pts - ap->GetNoPlayMs();
			//cout << "audio pts = " << pts << endl;

			//�ز���
			int size = res->Resample(frame, pcm);
			//������Ƶ
			while (!isExit) {
				if (size <= 0) break;
				//����δ���꣬�ռ䲻��
				if ((ap->GetFree() < size) || isPause) {//����isPause,����ͣʱ��Qt��Ƶ�豸�л��������Ҳ������ͣ
					msleep(1);
					continue;
				}
				ap->Write(pcm, size);
				break;
			}
		}
		
		amux.unlock();
	}

	delete []pcm;
}