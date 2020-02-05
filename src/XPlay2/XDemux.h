#pragma once
#include <mutex>
struct AVFormatContext;
struct AVPacket;
struct AVCodecParameters;

class XDemux
{
public:
	XDemux();
	virtual ~XDemux();

	//打开媒体文件，或者流媒体rtmp http rtsp
	virtual bool Open(const char* url);

	virtual bool IsAudio(AVPacket* pkt);

	//空间需要调用者释放,释放AVPacket对象空间和数据空间 av_packet_free();
	virtual AVPacket *Read();

	//媒体总时长ms
	int totalMs = 0;

	//获取视频参数，返回的空间需要清理  avcodec_parameters_free()
	AVCodecParameters *CopyVPara();

	//获取音频参数，返回的空间需要清理 avcodec_parameters_free()
	AVCodecParameters *CopyAPara();

	//seek位置 pos 0.0~1.0
	virtual bool Seek(double pos);

	//清空读取缓存
	virtual void Clear();

	virtual void Close();

protected:
	std::mutex mux;

	//解封装上下文
	AVFormatContext *ic = NULL;

	//音视频索引，读取时区分音视频
	int videoStream = 0;
	int audioStream = 1;
};
