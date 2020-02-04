#pragma once
#include <mutex>
struct AVFormatContext;

class XDemux
{
public:
	XDemux();
	virtual ~XDemux();

	//打开媒体文件，或者流媒体rtmp http rtsp
	virtual bool Open(const char* url);

	//媒体总时长ms
	int totalMs = 0;

protected:
	//解封装上下文
	AVFormatContext *ic = NULL;

	//音视频索引，读取时区分音视频
	int videoStream = 0;
	int audioStream = 1;
};
