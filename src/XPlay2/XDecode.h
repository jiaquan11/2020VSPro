#pragma once
#include <mutex>

struct AVCodecParameters;
struct AVCodecContext;
struct AVPacket;
struct AVFrame;

class XDecode
{
public:
	XDecode();
	virtual ~XDecode();

	bool isAudio = false;
	//�򿪽�����,���ܳɹ�����ͷ�para�ռ�
	virtual bool Open(AVCodecParameters *para);

	//���͵������̣߳����ܳɹ�����ͷ�pkt�ռ�(�����ý������)
	virtual bool Send(AVPacket* pkt);

	//��ȡ�������ݣ�һ��send������Ҫ���Recv,��ȡ�����е�����Send NULL �ٶ��Recv
	//ÿ�θ���һ�ݣ��ɵ������ͷ�av_frame_free
	virtual AVFrame* Recv();

	virtual void Clear();

	virtual void Close();

protected:
	AVCodecContext* avc = 0;
	std::mutex mux;
};

