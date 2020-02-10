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

//停止线程，清理资源
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
		//一次send,多次recv
		while (!isExit){
			AVFrame* frame = decode->Recv();
			if (!frame) break;

			//减去缓冲中未播放的时间
			pts = decode->pts - ap->GetNoPlayMs();
			//cout << "audio pts = " << pts << endl;

			//重采样
			int size = res->Resample(frame, pcm);
			//播放音频
			while (!isExit) {
				if (size <= 0) break;
				//缓冲未播完，空间不够
				if ((ap->GetFree() < size) || isPause) {//这里isPause,当暂停时，Qt音频设备中缓存的数据也马上暂停播放
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