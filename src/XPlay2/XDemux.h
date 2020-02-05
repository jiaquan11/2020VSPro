#pragma once
#include <mutex>
struct AVFormatContext;
struct AVPacket;
struct AVCodecParameters;

class XDemux
{
public:
	XDemux();
	virtual ~XDemux();

	//��ý���ļ���������ý��rtmp http rtsp
	virtual bool Open(const char* url);

	virtual bool IsAudio(AVPacket* pkt);

	//�ռ���Ҫ�������ͷ�,�ͷ�AVPacket����ռ�����ݿռ� av_packet_free();
	virtual AVPacket *Read();

	//ý����ʱ��ms
	int totalMs = 0;

	//��ȡ��Ƶ���������صĿռ���Ҫ����  avcodec_parameters_free()
	AVCodecParameters *CopyVPara();

	//��ȡ��Ƶ���������صĿռ���Ҫ���� avcodec_parameters_free()
	AVCodecParameters *CopyAPara();

	//seekλ�� pos 0.0~1.0
	virtual bool Seek(double pos);

	//��ն�ȡ����
	virtual void Clear();

	virtual void Close();

protected:
	std::mutex mux;

	//���װ������
	AVFormatContext *ic = NULL;

	//����Ƶ��������ȡʱ��������Ƶ
	int videoStream = 0;
	int audioStream = 1;
};
