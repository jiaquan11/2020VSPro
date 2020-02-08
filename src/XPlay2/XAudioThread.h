#pragma once
#include <QThread>
#include <mutex>
#include <list>

struct AVCodecParameters;
class XDecode;
class XAudioPlay;
class XResample;
class AVPacket;

class XAudioThread : public QThread
{
public:
	XAudioThread();
	virtual ~XAudioThread();

	//当前音频播放的pts
	long long pts = 0;

	//不管成功与否都清理
	virtual bool Open(AVCodecParameters* para, int sampleRate, int channels);
	virtual void Push(AVPacket* pkt);

	void run();

	//最大队列
	int maxList = 100;
	bool isExit = false;
protected:
	std::list<AVPacket*> packs;
	std::mutex mux;

	XDecode *decode = NULL;
	XAudioPlay *ap = NULL;
	XResample *res = NULL;
};

