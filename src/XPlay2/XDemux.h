#pragma once
#include <mutex>
struct AVFormatContext;
struct AVPacket;

class XDemux
{
public:
	XDemux();
	virtual ~XDemux();

	//��ý���ļ���������ý��rtmp http rtsp
	virtual bool Open(const char* url);

	//�ռ���Ҫ�������ͷ�,�ͷ�AVPacket����ռ�����ݿռ� av_packet_free();
	virtual AVPacket *Read();

	//ý����ʱ��ms
	int totalMs = 0;

protected:
	std::mutex mux;

	//���װ������
	AVFormatContext *ic = NULL;

	//����Ƶ��������ȡʱ��������Ƶ
	int videoStream = 0;
	int audioStream = 1;
};
