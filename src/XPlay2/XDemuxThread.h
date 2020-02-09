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
	long long pts = 0;
	long long totalMs = 0;
	bool isPause = false;

	//���������߳�
	virtual void Start();
	void run();
	//�������󲢴�
	virtual bool Open(const char* url, IVideoCall* call);

	virtual void Clear();
	//�ر��̣߳�������Դ
	virtual void Close();

	virtual void Seek(double pos);

	void SetPause(bool isPause);

protected:
	std::mutex mux;

	XDemux *demux = 0;
	XVideoThread *vt = 0;
	XAudioThread *at = 0;
};

