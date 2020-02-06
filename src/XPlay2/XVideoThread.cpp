#include "XVideoThread.h"
#include "XDecode.h"
#include <iostream>
using namespace std;

XVideoThread::XVideoThread(){

}

XVideoThread::~XVideoThread(){
	//�ȴ��߳��˳�
	isExit = true;
	wait();
}

//���ܳɹ��������
bool XVideoThread::Open(AVCodecParameters* para, IVideoCall* call, int width, int height) {
	if (!para) return false;
	mux.lock();
	//��ʼ����ʾ����
	this->call = call;
	if (call) {
		call->Init(width, height);
	}

	//�򿪽�����
	if (!decode) {
		decode = new XDecode();
	}
	int ret = true;
	if (!decode->Open(para)) {
		cout << "Video XDecode open failed" << endl;
		ret = false;
	}

	mux.unlock();
	cout << "XVideoThread open ret: " << ret << endl;

	return ret;
}

void XVideoThread::Push(AVPacket* pkt) {
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

void XVideoThread::run() {
	while (!isExit) {
		mux.lock();
		//û������
		if (packs.empty() || !decode) {
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
		while (!isExit) {
			AVFrame* frame = decode->Recv();
			if (!frame) break;
			//��ʾ��Ƶ
			if (call) {
				call->Repaint(frame);
			}
		}

		mux.unlock();
	}
}
