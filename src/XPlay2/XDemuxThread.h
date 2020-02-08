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

	//���������߳�
	virtual void Start();
	void run();
	//�������󲢴�
	virtual bool Open(const char* url, IVideoCall* call);

protected:
	std::mutex mux;

	XDemux *demux = 0;
	XVideoThread *vt = 0;
	XAudioThread *at = 0;
};

