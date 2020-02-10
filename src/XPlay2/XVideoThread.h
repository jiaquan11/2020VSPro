#pragma once
#include <list>
#include <mutex>
#include <QThread>
#include "IVideoCall.h"
#include "XDecodeThread.h"

struct AVPacket;
class XDecode;
struct AVCodecParameters;

//解码和显示视频
class XVideoThread : public XDecodeThread
{
public:
	XVideoThread();
	virtual ~XVideoThread();

	//不管成功与否都清理
	virtual bool Open(AVCodecParameters* para, IVideoCall* call, int width, int height);
	
	//解码pts,如果接收到的解码数据pts >= seekpts return true  并且显示画面
	virtual bool RepaintPts(AVPacket* pkt, long long seekpts);

	void run();

	//同步时间，由外部传入
	long long synpts = 0;

	bool isPause = false;
	void SetPause(bool isPause);

protected:
	std::mutex vmux;
	IVideoCall* call = NULL;
};

