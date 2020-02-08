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

	//��ǰ��Ƶ���ŵ�pts
	long long pts = 0;

	//���ܳɹ��������
	virtual bool Open(AVCodecParameters* para, int sampleRate, int channels);
	virtual void Push(AVPacket* pkt);

	void run();

	//������
	int maxList = 100;
	bool isExit = false;
protected:
	std::list<AVPacket*> packs;
	std::mutex mux;

	XDecode *decode = NULL;
	XAudioPlay *ap = NULL;
	XResample *res = NULL;
};

