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

	//������
	int maxList = 100;
	bool isExit = false;

	virtual void Push(AVPacket* pkt);
	//ȡ��һ֡���ݣ�����ջ�����û�з���NULL
	virtual AVPacket* Pop();

	//�������
	virtual void Clear();

protected:
	std::list<AVPacket*> packs;
	std::mutex mux;

	XDecode *decode = NULL;
};

