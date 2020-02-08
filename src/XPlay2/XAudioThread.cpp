#include "XAudioThread.h"
#include "XAudioPlay.h"
#include "XResample.h"
#include "XDecode.h"
#include <iostream>
using namespace std;

XAudioThread::XAudioThread(){

}


XAudioThread::~XAudioThread(){
	//�ȴ��߳��˳�
	isExit = true;
	wait();
}

bool XAudioThread::Open(AVCodecParameters* para, int sampleRate, int channels) {
	if (!para) return false;
	mux.lock();
	pts = 0;

	if (!decode) {
		decode = new XDecode();
	}
	if (!res) {
		res = new XResample();
	}
	if (!ap) {
		ap = XAudioPlay::Get();
	}
	bool ret = true;
	if (!res->Open(para, false)) {
		cout << "XResample open failed" << endl;
		ret = false;
	}
	ap->sampleRate = sampleRate;
	ap->channels = channels;

	if (!ap->Open()) {
		cout << "XAudioPlay open failed" << endl;
		//mux.unlock();
		//return false;

		ret = false;
	}

	if (!decode->Open(para)) {
		cout << "Audio XDecode open failed" << endl;
		ret = false;
	}

	mux.unlock();
	cout << "XAudioThread open ret: " <<ret<< endl;

	return ret; 
}

void XAudioThread::Push(AVPacket* pkt) {
	if (!pkt)
		return;
	
	//����
	while (!isExit) {
		mux.lock();
		if (packs.size() < maxList) {
			packs.push_back(pkt);
			mux.unlock();
			break;
		}
		mux.unlock();
		msleep(1);
	}
}

void XAudioThread::run() {
	unsigned char* pcm = new unsigned char[1024*1024*10];
	while (!isExit) {
		mux.lock();
		//û������
		if (packs.empty() || !decode || !res || !ap) {
			mux.unlock();
			msleep(1);
			continue;
		}

		AVPacket* pkt = packs.front();
		packs.pop_front();

		bool ret = decode->Send(pkt);
		if (!ret) {
			mux.unlock();
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
				if (ap->GetFree() < size) {
					msleep(1);
					continue;
				}
				ap->Write(pcm, size);
				break;
			}
		}
		
		mux.unlock();
	}

	delete []pcm;
}