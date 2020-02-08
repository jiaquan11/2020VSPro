#pragma once
#include <QThread>
#include <mutex>
#include <list>

struct AVCodecParameters;
class XAudioPlay;
class XResample;
#include "XDecodeThread.h"

class XAudioThread : public XDecodeThread
{
public:
	XAudioThread();
	virtual ~XAudioThread();

	//当前音频播放的pts
	long long pts = 0;

	//不管成功与否都清理
	virtual bool Open(AVCodecParameters* para, int sampleRate, int channels);

	//停止线程，清理资源
	virtual void Close();

	void run();

protected:
	std::mutex amux;

	XAudioPlay *ap = NULL;
	XResample *res = NULL;
};

