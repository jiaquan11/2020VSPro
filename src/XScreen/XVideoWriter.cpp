#include "XVideoWriter.h"
#include <iostream>
using namespace std;

extern "C" {
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/opt.h>
#include <libswresample/swresample.h>
}

#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "swscale.lib")
#pragma comment(lib, "swresample.lib")

class CXVideoWriter : public XVideoWriter {
public:
	AVFormatContext* oc = NULL;//封装mp4输出上下文
	AVCodecContext* vc = NULL;//视频编码器上下文
	AVStream* vs = NULL;//视频流
	AVCodecContext* ac = NULL;//音频编码器上下文
	AVStream* as = NULL;//音频流
	SwsContext* vsc = NULL;//像素转换上下文
	SwrContext* asc = NULL;//音频重采样上下文
	AVFrame* yuv = NULL;//输出yuv
	AVFrame* pcm = NULL;//输出pcm
	int vpts = 0;//视频pts
	int apts = 0;//音频pts;

	void Close() {
		if (oc) {
			avformat_close_input(&oc);
		}
		if (vc) {
			avcodec_close(vc);
			avcodec_free_context(&vc);
		}
		if (ac) {
			avcodec_close(ac);
			avcodec_free_context(&ac);
		}
		if (vsc) {
			sws_freeContext(vsc);
			vsc = NULL;
		}
		if (asc) {
			swr_free(&asc);
		}
		if (yuv) {
			av_frame_free(&yuv);
		}
		if (pcm) {
			av_frame_free(&pcm);
		}
	}

	bool Init(const char* file) {
		Close();

		//封装文件输出上下文
		avformat_alloc_output_context2(&oc, NULL, NULL, file);
		if (!oc) {
			cerr << "avformat_alloc_output_context2 failed" << endl;
			return false;
		}

		filename = file;
		return true;
	}

	bool WriteHead() {
		if (!oc) return false;

		//打开io
		int ret = avio_open(&oc->pb, filename.c_str(), AVIO_FLAG_WRITE);
		if (ret != 0) {
			cerr << "avio_open failed!" << endl;
			return false;
		}

		//写入封装头
		ret = avformat_write_header(oc, NULL);
		if (ret != 0) {
			cerr << "avformat_write_header failed!" << endl;
			return false;
		}

		cout << "write " << filename << " head success!" << endl;
		return true;
	}

	bool WriteEnd() {
		if (!oc || !oc->pb) return false;
		//写入尾部信息索引
		if (av_write_trailer(oc) != 0) {
			cerr << "av_write_trailer failed!" << endl;
			return false;
		}
		//关闭输入io
		/*
		avio_closep可以关闭io,同时内部置NULL,否则avformat_close_input又会对oc->pb释放一遍，
		导致崩溃
		*/
		if (avio_closep(&oc->pb) != 0) {
			cerr << "avio_close failed!" << endl;
			return false;
		}
		cout << "WriteEnd success!" << endl;

		return true;
	}

	bool AddVideoStream() {
		if (!oc) {
			return false;
		}
		//1.视频编码器创建
		AVCodec *codec = avcodec_find_encoder(AV_CODEC_ID_H264);
		if (!codec) {
			cerr << "avcodec_find_encoder AV_CODEC_ID_H264 failed" << endl;
			return false;
		}
		vc = avcodec_alloc_context3(codec);
		if (!vc) {
			cerr << "avcodec_alloc_context3 failed!" << endl;
			return false;
		}

		//比特率，压缩后每秒大小
		vc->bit_rate = vBitrate;
		vc->width = outWidth;
		vc->height = outHeight;
		//时间基数
		vc->time_base = { 1, outFPS };
		vc->framerate = { outFPS, 1 };
		//画面组大小，多少帧一个关键帧
		vc->gop_size = 60;
		vc->max_b_frames = 0;

		vc->pix_fmt = AV_PIX_FMT_YUV420P;
		vc->codec_id = AV_CODEC_ID_H264;
		av_opt_set(vc->priv_data, "preset", "superfast", 0);
		vc->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

		//打开编码器
		int ret = avcodec_open2(vc, codec, NULL);
		if (ret != 0) {
			cerr << "avcodec_open2 failed" << endl;
			return false;
		}
		cout << "avcodec_open2 success!" << endl;

		//添加视频流到输出上下文
		vs = avformat_new_stream(oc, NULL);
		vs->codecpar->codec_tag = 0;
		avcodec_parameters_from_context(vs->codecpar, vc);

		av_dump_format(oc, 0, filename.c_str(), 1);

		//像素(尺寸)转换上下文 rgb to yuv
		vsc = sws_getCachedContext(vsc, 
			inWidth, inHeight, (AVPixelFormat)inPixFmt,
			outWidth, outHeight, AV_PIX_FMT_YUV420P,
			SWS_BICUBIC, NULL, NULL, NULL);
		if (!vsc) {
			cerr << "sws_getCachedContext failed" << endl;
			return false;
		}

		if (!yuv) {
			yuv = av_frame_alloc();
			yuv->format = AV_PIX_FMT_YUV420P;
			yuv->width = outWidth;
			yuv->height = outHeight;
			yuv->pts = 0;
			int ret = av_frame_get_buffer(yuv, 32);
			if (ret != 0) {
				cerr << "av_frame_get_buffer failed" << endl;
				return false;
			}
		}

		return true;
	}

	bool AddAudioStream() {
		if (!oc) return false;

		//1.找到音频解码
		AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_AAC);
		if (!codec) {
			cerr << "avcodec_find_encoder AV_CODEC_ID_AAC failed" << endl;
			return false;
		}
		//2.创建并打开音频编码器
		ac = avcodec_alloc_context3(codec);
		if (!ac) {
			cerr << "avcodec_alloc_context3 failed" << endl;
			return false;
		}
		ac->bit_rate = aBitrate;
		ac->sample_rate = outSampleRate;
		ac->sample_fmt = (AVSampleFormat)outSampleFmt;
		ac->channels = outChannels;
		ac->channel_layout = av_get_default_channel_layout(outChannels);
		ac->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
		int ret = avcodec_open2(ac, codec, NULL);
		if (ret != 0) {
			avcodec_free_context(&ac);
			cerr << "avcodec_open2 failed" << endl;
			return false;
		}
		cout << "avcodec_open2 AV_CODEC_ID_AAC success!" << endl;

		//3.新增音频流
		as = avformat_new_stream(oc, NULL);
		if (!as) {
			cerr << "avformat_new_stream failed" << endl;
			return false;
		}
		as->codecpar->codec_tag = 0;
		avcodec_parameters_from_context(as->codecpar, ac);

		av_dump_format(oc, 0, filename.c_str(), 1);

		//4 音频重采样上下文创建
		asc = swr_alloc_set_opts(asc, 
			ac->channel_layout, ac->sample_fmt, ac->sample_rate,//输出格式
			av_get_default_channel_layout(inChannels), (AVSampleFormat)inSampleFmt, inSampleRate,
			0, 0);
		ret = swr_init(asc);
		if (ret != 0) {
			cerr << "swr_init failed" << endl;
			return false;
		}

		//音频重采样后输出AVFrame
		if (!pcm) {
			pcm = av_frame_alloc();
			pcm->format = ac->sample_fmt;
			pcm->channels = ac->channels;
			pcm->channel_layout = ac->channel_layout;
			pcm->nb_samples = nb_sample;//一帧音频的样本数量
			ret = av_frame_get_buffer(pcm, 0);
			if (ret < 0) {
				cerr << "av_frame_get_buffer failed" << endl;
				return false;
			}
			cout << "audio AVFrame create success!" << endl;
		}
		return true;
	}

	AVPacket* EncodeVideo(const unsigned char* rgb) {
		if (!oc || !vsc || !yuv) return NULL;

		uint8_t *indata[AV_NUM_DATA_POINTERS] = { 0 };
		indata[0] = (uint8_t *)rgb;
		int insize[AV_NUM_DATA_POINTERS] = { 0 };
		insize[0] = inWidth * 4;

		//rgb to yuv
		int h = sws_scale(vsc, indata, insize, 0, inHeight,
			yuv->data, yuv->linesize);
		if (h < 0) {
			return NULL;
		}

		//encode
		yuv->pts = vpts;
		vpts++;
		int ret = avcodec_send_frame(vc, yuv);
		if (ret != 0) {
			return NULL;
		}
		AVPacket* pkt = av_packet_alloc();
		av_init_packet(pkt);
		ret = avcodec_receive_packet(vc, pkt);
		if ((ret != 0) || (pkt->size <= 0)) {
			av_packet_free(&pkt);
			return NULL;
		}

		/*
		这里很重要
		将packet中AVCodecContext中的timebase转换为流AVStream中的timebase
		然后再进行写文件。两个timebase的值是不一样的，写文件时必须将packet转换为
		各自流的timebase,视频文件才能正常合成
		*/
		av_packet_rescale_ts(pkt, vc->time_base, vs->time_base);

		pkt->stream_index = vs->index;
		return pkt;
	}

	AVPacket* EncodeAudio(const unsigned char* d) {
		if (!oc || !asc || !pcm) return NULL;

		//1.音频重采样
		const uint8_t* data[AV_NUM_DATA_POINTERS] = { 0 };
		data[0] = (uint8_t*)d;
		int len = swr_convert(asc, pcm->data, pcm->nb_samples,
			data, pcm->nb_samples);
		
		//2 音频的编码
		int ret = avcodec_send_frame(ac, pcm);
		if (ret != 0) {
			return NULL;
		}
		AVPacket* pkt = av_packet_alloc();
		av_init_packet(pkt);
		ret = avcodec_receive_packet(ac, pkt);
		if (ret != 0){
			av_packet_free(&pkt);
			return NULL;
		}

		cout << pkt->size << "|";
		pkt->stream_index = as->index;
		pkt->pts = apts;
		pkt->dts = pkt->pts;
		/*
		这里很重要，对于音频，这里av_rescale_q可以根据参数计算出每个音频包的长度，
		然后进行累加。
		av_rescale_q换算AVCodecContext和AVStream的时间基
		*/
		apts += av_rescale_q(pcm->nb_samples, { 1, ac->sample_rate }, as->time_base);

		return pkt;
	}

	/*
	为了同步音视频的写入
	*/
	bool IsVideoBefore() {
		if (!oc || !as || !vs) {
			return false;
		}
		/*
		这里很重要，是为了比较音频和视频的当前时间戳，然后再进行同步决定是否写入
		文件中
		*/
		int ret = av_compare_ts(vpts, vc->time_base, apts, ac->time_base);
		if (ret <= 0) {
			return true;
		}

		return false;
	}

	bool WriteFrame(AVPacket* pkt) {
		if (!oc || !pkt || (pkt->size <= 0)) return false;
		
		if (av_interleaved_write_frame(oc, pkt) != 0) return false;

		return true;
	}
};

XVideoWriter::XVideoWriter(){

}

XVideoWriter::~XVideoWriter(){

}

XVideoWriter *XVideoWriter::Get(unsigned int index) {
	static bool isFirst = true;
	if (isFirst) {
		av_register_all();
		avcodec_register_all();
		isFirst = false;
	}

	static CXVideoWriter wrs[65535];
	return &wrs[index];
}