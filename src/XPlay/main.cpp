#include "XPlay.h"
#include <QtWidgets/QApplication>
#include "XFFmpeg.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "swscale.lib")

static double r2d(AVRational r) {
	return r.num == 0 || r.den == 0 ? 0 : (double)r.num / (double)r.den;
}

int main(int argc, char *argv[])
{
	//av_register_all();
	//char* path = "testvideo1.mp4";

	//AVFormatContext* ic = NULL;
	//int ret = avformat_open_input(&ic, path, 0, 0);
	//if (ret != 0) {
	//	char buf[1024] = { 0 };
	//	av_strerror(ret, buf, sizeof(buf));
	//	printf("open %s failed: %s\n", path, buf);
	//	getchar();
	//	return -1;
	//}

	//int totalSec = ic->duration / AV_TIME_BASE;
	//printf("file totalSec is %d-%d\n", totalSec / 60, totalSec % 60);
	//
	//if (XFFmpeg::Get()->Open("testvideo1.mp4")) {
	//	printf("open success!\n");
	//}
	//else {
	//	printf("open failed!:%s", XFFmpeg::Get()->GetError().c_str());
	//	getchar();
	//	return -1;
	//}

	//int videoStream = 0;
	//AVCodecContext *videoCtx = NULL;
	//for (int i = 0; i < ic->nb_streams; i++) {
	//	AVCodecContext* enc = ic->streams[i]->codec;
	//	
	//	if (enc->codec_type == AVMEDIA_TYPE_VIDEO) {
	//		videoCtx = enc;
	//		videoStream = i;
	//		AVCodec* codec = avcodec_find_decoder(enc->codec_id);
	//		if (!codec) {
	//			printf("video code not find!\n");
	//			return -1;
	//		}

	//		int err = avcodec_open2(enc, codec, NULL);
	//		if (err != 0) {
	//			char buf[1024] = { 0 };
	//			av_strerror(err, buf, sizeof(buf));
	//			printf(buf);
	//			return -2;
	//		}
	//		printf("open codec success!\n");
	//	}
	//}

	//AVFrame* yuv = av_frame_alloc();
	//SwsContext *cCtx = NULL;
	//int outwidth = 640;
	//int outheight = 480;
	//char* rgb = new char[outwidth * outheight * 4];

	//for (;;){
	//	AVPacket pkt;
	//	ret = av_read_frame(ic, &pkt);
	//	if (ret != 0) {
	//		break;
	//	}
	//	if (pkt.stream_index != videoStream) {
	//		continue;
	//	}

	//	pkt.pts = pkt.pts * r2d(ic->streams[pkt.stream_index]->time_base) * 1000;
	//	/*int got_picture = 0;
	//	int ret = avcodec_decode_video2(videoCtx, yuv, &got_picture, &pkt);
	//	if (got_picture) {
	//		printf("[%d]", ret);
	//	}*/
	//	int ret = avcodec_send_packet(videoCtx, &pkt);
	//	if (ret != 0) {
	//		av_packet_unref(&pkt);
	//		continue;
	//	}
	//	ret = avcodec_receive_frame(videoCtx, yuv);
	//	if (ret != 0) {
	//		av_packet_unref(&pkt);
	//		continue;
	//	}
	//	printf("[D]");

	//	cCtx = sws_getCachedContext(cCtx, videoCtx->width,
	//		videoCtx->height,
	//		videoCtx->pix_fmt,
	//		outwidth, outheight,
	//		AV_PIX_FMT_BGRA,
	//		SWS_BICUBIC,
	//		NULL, NULL, NULL);
	//	if (!cCtx) {
	//		printf("sws_getCachedContext failed!\n");
	//		break;
	//	}

	//	uint8_t *data[AV_NUM_DATA_POINTERS] = {0};
	//	data[0] = (uint8_t*)rgb;
	//	int linesize[AV_NUM_DATA_POINTERS] = { 0 };
	//	linesize[0] = outwidth * 4;
	//	int h = sws_scale(cCtx, yuv->data, yuv->linesize, 0, videoCtx->height,
	//		data,
	//		linesize);
	//	if (h > 0) {
	//		printf("(%d)", h);
	//	}
	//	printf("pts = %lld\n", pkt.pts);

	//	av_packet_unref(&pkt);
	//}
	//
	//if (cCtx) {
	//	sws_freeContext(cCtx);
	//	cCtx = NULL;
	//}
	//avformat_close_input(&ic);
	//ic = NULL;

	if (XFFmpeg::Get()->Open("testvideo1.mp4")) {
		printf("open success!\n");
	}else {
		printf("open failed! %s", XFFmpeg::Get()->GetError().c_str());
		getchar();
		return -1;
	}

	char *rgb = new char[800 * 600 * 4];
	for (;;){
		AVPacket pkt = XFFmpeg::Get()->Read();
		if (pkt.size == 0) break;
		printf("pts = %lld\n", pkt.pts);

		if (pkt.stream_index != XFFmpeg::Get()->videoStream) {
			av_packet_unref(&pkt);
			continue;
		}
		AVFrame* yuv = XFFmpeg::Get()->Decode(&pkt);
		if (yuv) {
			printf("[D]");
			XFFmpeg::Get()->ToRGB(yuv, rgb, 800, 600);
		}
		
		av_packet_unref(&pkt);
	}


	QApplication a(argc, argv);
	XPlay w;
	w.show();
	return a.exec();
}
