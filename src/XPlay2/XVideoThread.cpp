#include "XVideoThread.h"
#include "XDecode.h"
#include <iostream>
using namespace std;

XVideoThread::XVideoThread(){

}

XVideoThread::~XVideoThread(){

}

//不管成功与否都清理
bool XVideoThread::Open(AVCodecParameters* para, IVideoCall* call, int width, int height) {
	if (!para) return false;
	
	Clear();

	vmux.lock();

	synpts = 0;

	//初始化显示窗口
	this->call = call;
	if (call) {
		call->Init(width, height);
	}
	vmux.unlock();

	int ret = true;
	if (!decode->Open(para)) {
		cout << "Video XDecode open failed" << endl;
		ret = false;
	}

	cout << "XVideoThread open ret: " << ret << endl;

	return ret;
}

void XVideoThread::SetPause(bool isPause) {
	vmux.lock();
	this->isPause = isPause;
	vmux.unlock();
}

//解码pts,如果接收到的解码数据pts >= seekpts return true  并且显示画面
bool XVideoThread::RepaintPts(AVPacket* pkt, long long seekpts) {
	vmux.lock();

	bool ret = decode->Send(pkt);
	if (!ret) {
		vmux.unlock();
		return true;//表示结束解码
	}
		
	AVFrame* frame = decode->Recv();
	if (!frame) {
		vmux.unlock();
		return false;
	}
		
	//到达位置
	if (decode->pts >= seekpts) {
		if (call) 
			call->Repaint(frame);

		vmux.unlock();
		return true;
	}

	XFreeFrame(&frame);

	vmux.unlock();
	return false;
}

void XVideoThread::run() {
	while (!isExit) {
		vmux.lock();
		if (this->isPause) {
			vmux.unlock();
			msleep(5);
			continue;
		}

		//音视频同步
		//cout << "audio synpts: " << synpts << " video pts: " << decode->pts << endl;
		if ((synpts > 0) && (synpts < decode->pts)) {
			vmux.unlock();
			msleep(1);
			continue;
		}

		AVPacket* pkt = Pop();

		bool ret = decode->Send(pkt);
		if (!ret) {
			vmux.unlock();
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

		vmux.unlock();
	}
}