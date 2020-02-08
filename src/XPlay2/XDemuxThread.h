#pragma once
#include <QThread>
#include <mutex>

#include "IVideoCall.h"
class XDemux;
class XVideoThread;
class XAudioThread;

class XDemuxThread : public QThread
{
public:
	XDemuxThread();
	virtual ~XDemuxThread();
	
	bool isExit = false;

	//启动所有线程
	virtual void Start();
	void run();
	//创建对象并打开
	virtual bool Open(const char* url, IVideoCall* call);

protected:
	std::mutex mux;

	XDemux *demux = 0;
	XVideoThread *vt = 0;
	XAudioThread *at = 0;
};

