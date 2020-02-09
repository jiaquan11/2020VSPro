#pragma once
#include <list>
#include <mutex>
#include <QThread>
#include "IVideoCall.h"
#include "XDecodeThread.h"

struct AVPacket;
class XDecode;
struct AVCodecParameters;

//�������ʾ��Ƶ
class XVideoThread : public XDecodeThread
{
public:
	XVideoThread();
	virtual ~XVideoThread();

	//���ܳɹ��������
	virtual bool Open(AVCodecParameters* para, IVideoCall* call, int width, int height);
	
	//����pts,������յ��Ľ�������pts >= seekpts return true  ������ʾ����
	virtual bool RepaintPts(AVPacket* pkt, long long seekpts);

	void run();

	//ͬ��ʱ�䣬���ⲿ����
	long long synpts = 0;

	bool isPause = false;
	void SetPause(bool isPause);

protected:
	std::mutex vmux;
	IVideoCall* call = NULL;
};

