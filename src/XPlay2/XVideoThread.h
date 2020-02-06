#pragma once
#include <list>
#include <mutex>
#include <QThread>
#include "IVideoCall.h"

struct AVPacket;
class XDecode;
struct AVCodecParameters;

//解码和显示视频
class XVideoThread : public QThread
{
public:
	XVideoThread();
	virtual ~XVideoThread();

	//不管成功与否都清理
	virtual bool Open(AVCodecParameters* para, IVideoCall* call, int width, int height);
	virtual void Push(AVPacket* pkt);
	void run();

	//最大队列
	int maxList = 100;
	bool isExit = false;

protected:
	std::list<AVPacket*> packs;
	std::mutex mux;

	XDecode *decode = NULL;

	IVideoCall* call = NULL;
};

