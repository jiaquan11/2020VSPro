#pragma once
#include <mutex>
struct AVFormatContext;

class XDemux
{
public:
	XDemux();
	virtual ~XDemux();

	//��ý���ļ���������ý��rtmp http rtsp
	virtual bool Open(const char* url);

	//ý����ʱ��ms
	int totalMs = 0;

protected:
	//���װ������
	AVFormatContext *ic = NULL;

	//����Ƶ��������ȡʱ��������Ƶ
	int videoStream = 0;
	int audioStream = 1;
};
