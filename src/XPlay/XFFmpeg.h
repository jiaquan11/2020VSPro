#pragma once
#include <QMutex>
#include <string>

extern "C" {
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

class XFFmpeg
{
public:
	static XFFmpeg* Get() {
		static XFFmpeg ff;
		return &ff;
	}

	int totalMs = 0;
	int videoStream = 0;

	//////////////////////
	//打开视频文件，如果上次已经打开，会先关闭
	bool Open(const char* path);
	void Close();

	//返回值需要用户清理
	AVPacket Read();

	AVFrame *Decode(const AVPacket *pkt);

	bool ToRGB(const AVFrame* yuv, char* out, int outwidth, int outheight);

	std::string GetError();

	virtual ~XFFmpeg();

protected:
	char errorbuf[1024];
	QMutex mutex;

	AVFormatContext* ic = NULL;
	AVFrame *yuv = NULL;
	
	SwsContext *cCtx = NULL;

protected:
	XFFmpeg();
};

