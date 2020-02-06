#pragma once
#include <list>
#include <mutex>
#include <QThread>
#include "IVideoCall.h"

struct AVPacket;
class XDecode;
struct AVCodecParameters;

//�������ʾ��Ƶ
class XVideoThread : public QThread
{
public:
	XVideoThread();
	virtual ~XVideoThread();

	//���ܳɹ��������
	virtual bool Open(AVCodecParameters* para, IVideoCall* call, int width, int height);
	virtual void Push(AVPacket* pkt);
	void run();

	//������
	int maxList = 100;
	bool isExit = false;

protected:
	std::list<AVPacket*> packs;
	std::mutex mux;

	XDecode *decode = NULL;

	IVideoCall* call = NULL;
};

