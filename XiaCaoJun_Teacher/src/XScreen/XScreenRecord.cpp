#include "XScreenRecord.h"
#include "XCaptureThread.h"
#include "XAudioThread.h"
#include "XVideoWriter.h"

XScreenRecord::XScreenRecord(){

}

XScreenRecord::~XScreenRecord(){

}

bool XScreenRecord::Start(const char* filename) {
	if (!filename) return false;

	Stop();

	mutex.lock();
	isExit = false;
	//初始化屏幕录制
	XCaptureThread::Get()->fps = fps;//指定截屏帧率
	XCaptureThread::Get()->Start();

	//初始化音频录制
	XAudioThread::Get()->Start();

	//初始化编码器
	XVideoWriter::Get()->inWidth = XCaptureThread::Get()->width;//截屏的宽
	XVideoWriter::Get()->inHeight = XCaptureThread::Get()->height;//截屏的高
	XVideoWriter::Get()->outWidth = outWidth;//指定输出视频的宽
	XVideoWriter::Get()->outHeight = outHeight;//指定输出视频的高
	XVideoWriter::Get()->outFPS = fps;//指定输出帧率
	XVideoWriter::Get()->Init(filename);
	XVideoWriter::Get()->AddVideoStream();
	XVideoWriter::Get()->AddAudioStream();
	if (!XVideoWriter::Get()->WriteHead()) {
		mutex.unlock();

		Stop();
		return false;
	}

	mutex.unlock();

	start();//开启线程

	return true;
}

void XScreenRecord::Stop() {
	mutex.lock();
	isExit = true;
	mutex.unlock();

	wait();//等待线程结束

	mutex.lock();
	XVideoWriter::Get()->WriteEnd();
	XVideoWriter::Get()->Close();
	XCaptureThread::Get()->Stop();
	XAudioThread::Get()->Stop();
	mutex.unlock();
}

//线程运行函数
void XScreenRecord::run() {
	while (!isExit) {
		mutex.lock();
		//写入视频
		char* rgb = XCaptureThread::Get()->GetRGB();
		if (rgb) {
			AVPacket* p = XVideoWriter::Get()->EncodeVideo((unsigned char*)rgb);
			delete []rgb;
			XVideoWriter::Get()->WriteFrame(p);
		}
		
		//写入音频
		char* pcm = XAudioThread::Get()->GetPCM();
		if (pcm) {
			AVPacket* p = XVideoWriter::Get()->EncodeAudio((unsigned char*)pcm);
			delete []pcm;
			XVideoWriter::Get()->WriteFrame(p);
		}

		msleep(10);

		mutex.unlock();
	}
}
