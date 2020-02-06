#pragma once
struct AVCodecParameters;
struct AVFrame;
struct SwrContext;

#include <mutex>

class XResample
{
public:
	XResample();
	~XResample();

	int outFormat = 1;//AV_SAMPLE_FMT_S16

	//����������������һ�³��˲�����ʽ�����ΪS16 ���ͷ�para
	virtual bool Open(AVCodecParameters *para);

	virtual void Close();

	//�����ز�����Ĵ�С,���ܳɹ�����ͷ�indata�ռ�
	virtual int Resample(AVFrame* indata, unsigned char* data);

protected:
	std::mutex mux;

	SwrContext* actx = NULL;
};

