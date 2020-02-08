#pragma once
#include <list>
#include <mutex>
#include <QThread>
#include "IVideoCall.h"

struct AVPacket;
class XDecode;

class XDecodeThread : public QThread
{
public:
	XDecodeThread();
	virtual ~XDecodeThread();

	//最大队列
	int maxList = 100;
	bool isExit = false;

	virtual void Push(AVPacket* pkt);
	//取出一帧数据，并出栈，如果没有返回NULL
	virtual AVPacket* Pop();

	//清理队列
	virtual void Clear();

protected:
	std::list<AVPacket*> packs;
	std::mutex mux;

	XDecode *decode = NULL;
};

