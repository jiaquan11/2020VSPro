#pragma once
#include <mutex>
struct AVFormatContext;
struct AVPacket;

class XDemux
{
public:
	XDemux();
	virtual ~XDemux();

	//打开媒体文件，或者流媒体rtmp http rtsp
	virtual bool Open(const char* url);

	//空间需要调用者释放,释放AVPacket对象空间和数据空间 av_packet_free();
	virtual AVPacket *Read();

	//媒体总时长ms
	int totalMs = 0;

protected:
	std::mutex mux;

	//解封装上下文
	AVFormatContext *ic = NULL;

	//音视频索引，读取时区分音视频
	int videoStream = 0;
	int audioStream = 1;
};
