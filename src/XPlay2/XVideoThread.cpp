#include "XVideoThread.h"
#include "XDecode.h"
#include <iostream>
using namespace std;

XVideoThread::XVideoThread(){

}

XVideoThread::~XVideoThread(){
	//等待线程退出
	isExit = true;
	wait();
}

//不管成功与否都清理
bool XVideoThread::Open(AVCodecParameters* para, IVideoCall* call, int width, int height) {
	if (!para) return false;
	mux.lock();
	//初始化显示窗口
	this->call = call;
	if (call) {
		call->Init(width, height);
	}

	//打开解码器
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

	//阻塞
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
		//没有数据
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
		//一次send,多次recv
		while (!isExit) {
			AVFrame* frame = decode->Recv();
			if (!frame) break;
			//显示视频
			if (call) {
				call->Repaint(frame);
			}
		}

		mux.unlock();
	}
}
