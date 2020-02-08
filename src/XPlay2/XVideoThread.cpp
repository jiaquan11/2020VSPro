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

void XVideoThread::run() {
	while (!isExit) {
		vmux.lock();
		//音视频同步
		cout << "audio synpts: " << synpts << " video pts: " << decode->pts << endl;
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
