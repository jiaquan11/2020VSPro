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

	//��ǰ��Ƶ���ŵ�pts
	long long pts = 0;

	//���ܳɹ��������
	virtual bool Open(AVCodecParameters* para, int sampleRate, int channels);

	//ֹͣ�̣߳�������Դ
	virtual void Close();

	void run();

protected:
	std::mutex amux;

	XAudioPlay *ap = NULL;
	XResample *res = NULL;
};

