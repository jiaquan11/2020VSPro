#include "XResample.h"
#include <iostream>
using namespace std;

extern "C" {
#include <libswresample/swresample.h>
#include <libavcodec/avcodec.h>
}

#pragma comment(lib, "swresample.lib")

XResample::XResample(){

}

XResample::~XResample(){

}

//输出参数和输入参数一致除了采样格式，输出为S16
bool XResample::Open(AVCodecParameters *para, bool isClearPara) {
	if (!para)
		return false;
	
	mux.lock();

	//音频重采样,上下文初始化
	//if (!actx)
		//actx = swr_alloc();
	//如果actx为NULL会分配空间
	actx = swr_alloc_set_opts(actx,
		av_get_default_channel_layout(2),//输出格式
		(AVSampleFormat)outFormat,		//输出样本格式  AV_SAMPLE_FMT_S16
		para->sample_rate,				//输出采样率
		av_get_default_channel_layout(para->channels),//输入格式
		(AVSampleFormat)para->format,
		para->sample_rate,
		0, 0);

	if (isClearPara) {
		avcodec_parameters_free(&para);
	}
	
	int ret = swr_init(actx);
	mux.unlock();
	if (ret != 0) {
		char buf[1024] = { 0 };
		av_strerror(ret, buf, sizeof(buf) - 1);
		cout << "swr_init failed: " << buf << endl;
		return false;
	}

	return true;
}

void XResample::Close() {
	mux.lock();
	if (actx) {
		swr_free(&actx);
	}

	mux.unlock();
}

//返回重采样后的大小,不管成功与否都释放indata空间
int XResample::Resample(AVFrame* indata, unsigned char* d) {
	if (!indata)
		return 0;

	if (!d) {
		av_frame_free(&indata);
		return 0;
	}

	uint8_t* data[2] = { 0 };
	data[0] = d;
	int ret = swr_convert(actx,
		data, indata->nb_samples,		//输出
		(const uint8_t**)indata->data, indata->nb_samples  //输入
	);

	int outSize = ret * indata->channels * av_get_bytes_per_sample((AVSampleFormat)outFormat);

	av_frame_free(&indata);

	if (ret <= 0) 
		return ret;

	return outSize;
}