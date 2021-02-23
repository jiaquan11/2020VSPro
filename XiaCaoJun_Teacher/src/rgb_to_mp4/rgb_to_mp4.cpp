#include <iostream>
using namespace std;

extern "C" {
	#include <libavformat/avformat.h>
	#include <libswscale/swscale.h>
}

#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"swscale.lib")

int main() {
	char infile[] = "infile.rgb";
	char outfile[] = "out.mp4";

	av_register_all();
	avcodec_register_all();
	
	FILE* fp = fopen(infile, "rb");
	if (!fp) {
		cout << infile << "open failed!" << endl;
		getchar();
		return -1;
	}

	int width = 720;
	int height = 1280;
	int fps = 25;

	//1.create codec
	AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_H264);
	if (!codec) {
		cout << "avcodec_find_encoder AV_CODEC_ID_H264 failed!" << endl;
		getchar();
		return -1;
	}
	AVCodecContext* c = avcodec_alloc_context3(codec);
	if (!c) {
		cout << "avcodec_alloc_context3 failed!" << endl;
		getchar();
		return -1;
	}
	//压缩比特率
	c->bit_rate = 400000000;
	c->width = width;
	c->height = height;
	c->time_base = {1, fps};
	c->framerate = { fps, 1 };
	c->gop_size = 60;
	c->max_b_frames = 0;//不设置B帧编码
	c->pix_fmt = AV_PIX_FMT_YUV420P;
	c->codec_id = AV_CODEC_ID_H264;
	c->thread_count = 8;

	//全局的编码信息
	c->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

	int ret = avcodec_open2(c, codec, NULL);
	if (ret < 0) {
		cout << "avodec_open2 failed" << endl;
		getchar();
		return -1;
	}
	cout << "avodec_open2 success!" << endl;

	//2.create out context
	AVFormatContext* oc = NULL;
	avformat_alloc_output_context2(&oc, 0, 0, outfile);

	//3.add video stream
	AVStream *st = avformat_new_stream(oc, NULL);
	//st->codec = c;
	st->id = 0;
	st->codecpar->codec_tag = 0;
	avcodec_parameters_from_context(st->codecpar, c);

	cout << "==================================" << endl;
	av_dump_format(oc, 0, outfile, 1);
	cout << "======================================" << endl;

	//4 rgb to yuv
	SwsContext *ctx = NULL;
	ctx = sws_getCachedContext(ctx,
		width, height, AV_PIX_FMT_BGRA,
		width, height, AV_PIX_FMT_YUV420P,
		SWS_BICUBIC, NULL, NULL, NULL);

	//输入空间
	unsigned char *rgb = new unsigned char[width*height*4];
	//输出空间
	AVFrame* yuv = av_frame_alloc();
	yuv->format = AV_PIX_FMT_YUV420P;
	yuv->width = width;
	yuv->height = height;
	ret = av_frame_get_buffer(yuv, 32);
	if (ret < 0) {
		cout << "av_frame_get_buffer failed" << endl;
		getchar();
		return -1;
	}

	//5 write mp4 head
	ret = avio_open(&oc->pb, outfile, AVIO_FLAG_WRITE);
	if (ret < 0) {
		cout << "avio_open failed" << endl;
		getchar();
		return -1;
	}
	ret = avformat_write_header(oc, NULL);
	if (ret < 0) {
		cout << "avformat_write_header failed" << endl;
		getchar();
		return -1;
	}

	int p = 0;
	for (;;){
		int len = fread(rgb, 1, width*height * 4, fp);
		if (len <= 0) break;
		uint8_t *indata[AV_NUM_DATA_POINTERS] = { 0 };
		indata[0] = rgb;
		int inlinesize[AV_NUM_DATA_POINTERS] = { 0 };
		inlinesize[0] = width * 4;

		int h = sws_scale(ctx, indata, inlinesize, 0, height,
			yuv->data, yuv->linesize);
		if (h <= 0) {
			break;
		}

		//6 encode frame
		//yuv->pict_type = AV_PICTURE_TYPE_I;
		yuv->pts = p;
		p = p + 3600;//这里是数据强制写死的，根据的是调试的timebase为1/90000，根据设置的fps=25,所以每帧pts根据时间基准需要自加3600
		ret = avcodec_send_frame(c, yuv);
		if (ret != 0) {
			continue;
		}

		AVPacket pkt;
		av_init_packet(&pkt);
		ret = avcodec_receive_packet(c, &pkt);
		if (ret != 0) {
			continue;
		}

		/*
		这里很重要
		av_interleaved_write_frame可以代表

		//av_write_frame(oc, &pkt);
		//av_packet_unref(&pkt);
		这两行代码，同时av_interleaved_write_frame对于写入packet是进行
		packet dts进行缓冲写入的
		*/
		//av_write_frame(oc, &pkt);
		//av_packet_unref(&pkt);

		av_interleaved_write_frame(oc, &pkt);

		cout << "<" <<pkt.size<<">";
	}

	//写入视频索引
	av_write_trailer(oc);
	//关闭视频输出io
	avio_close(oc->pb);

	//清理封装输出上下文
	avformat_free_context(oc);

	//关闭编码器
	avcodec_close(c);
	//清理编码器上下文
	avcodec_free_context(&c);

	//清理视频缩放上下文
	sws_freeContext(ctx);

	cout << "============end===============" << endl;
	delete[]rgb;
	getchar();
	return 0;
}