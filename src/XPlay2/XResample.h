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

	//输出参数和输入参数一致除了采样格式，输出为S16 会释放para
	virtual bool Open(AVCodecParameters *para);

	virtual void Close();

	//返回重采样后的大小,不管成功与否都释放indata空间
	virtual int Resample(AVFrame* indata, unsigned char* data);

protected:
	std::mutex mux;

	SwrContext* actx = NULL;
};

