#pragma once
#include <mutex>

struct AVCodecParameters;
struct AVCodecContext;
struct AVPacket;
struct AVFrame;

class XDecode
{
public:
	XDecode();
	virtual ~XDecode();

	bool isAudio = false;
	//打开解码器,不管成功与否都释放para空间
	virtual bool Open(AVCodecParameters *para);

	//发送到解码线程，不管成功与否都释放pkt空间(对象和媒体内容)
	virtual bool Send(AVPacket* pkt);

	//获取解码数据，一次send可能需要多次Recv,获取缓冲中的数据Send NULL 再多次Recv
	//每次复制一份，由调用者释放av_frame_free
	virtual AVFrame* Recv();

	virtual void Clear();

	virtual void Close();

protected:
	AVCodecContext* avc = 0;
	std::mutex mux;
};

