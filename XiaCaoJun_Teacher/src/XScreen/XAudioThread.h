#pragma once
#include <QThread>
#include <QMutex>

class XAudioThread : protected QThread
{
public:
	int sampleRate = 44100;
	int channels = 2;
	int sampleByte = 2;
	int nbSample = 1024;

	int cacheSize = 10;

	virtual ~XAudioThread();

	static XAudioThread* Get() {
		static XAudioThread at;
		return &at;
	}
	void Start();
	void Stop();
	void run();
	//线程安全,返回空间由用户清理
	char* GetPCM();

protected:
	bool isExit = false;
	std::list<char*> pcms;
	QMutex mutex;

protected:
	XAudioThread();
};

