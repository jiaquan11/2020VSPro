#include <iostream>
using namespace std;

extern "C" {
	#include <libavformat/avformat.h>
	#include <libswscale/swscale.h>
	#include <libswresample/swresample.h>
}

#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "swscale.lib")
#pragma comment(lib, "swresample.lib")

int main() {
	char infile[] = "infile.pcm";
	char outfile[] = "out.aac";

	av_register_all();
	avcodec_register_all();
	
	//1.����Ƶ������
	AVCodec *codec = avcodec_find_encoder(AV_CODEC_ID_AAC);
	if (!codec) {
		cout << "avcodec_find_encoder error" << endl;
		getchar();
		return -1;
	}
	AVCodecContext* c = avcodec_alloc_context3(codec);
	if (!c) {
		cout << "avcodec_alloc_context3 error" << endl;
		getchar();
		return -1;
	}
	c->bit_rate = 64000;
	c->sample_rate = 44100;
	c->sample_fmt = AV_SAMPLE_FMT_FLTP;
	c->channel_layout = AV_CH_LAYOUT_STEREO;
	c->channels = 2;
	c->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

	int ret = avcodec_open2(c, codec, NULL);
	if (ret < 0) {
		cout << "avcodec_open2 error" << endl;
		getchar();
		return -1;
	}
	cout << "avcodec_open2 success!" << endl;

	//2 �������װ��������
	AVFormatContext* oc = NULL;
	avformat_alloc_output_context2(&oc, NULL, NULL, outfile);
	if (!oc) {
		cout << "avformat_alloc_output_context2 error" << endl;
		getchar();
		return -1;
	}

	AVStream* st = avformat_new_stream(oc, NULL);
	st->codecpar->codec_tag = 0;
	avcodec_parameters_from_context(st->codecpar, c);

	av_dump_format(oc, 0, outfile, 1);
	 
	//3 open io, write head
	ret = avio_open(&oc->pb, outfile, AVIO_FLAG_WRITE);
	if (ret < 0) {
		cout << "avio_open error" << endl;
		getchar();
		return -1;
	}
	ret = avformat_write_header(oc, NULL);

	//44100 16 2
	//4 ������Ƶ�ز���������
	SwrContext *actx = NULL;
	actx = swr_alloc_set_opts(actx,
		c->channel_layout, c->sample_fmt, c->sample_rate,//�����ʽ
		AV_CH_LAYOUT_STEREO, AV_SAMPLE_FMT_S16, 44100, 0, 0);//�����ʽ
	if (!actx) {
		cout << "swr_alloc_set_opts error" << endl;
		getchar();
		return -1;
	}

	ret = swr_init(actx);
	if (ret < 0) {
		cout << "swr_init error" << endl;
		getchar();
		return -1;
	}

	//5 ��������Ƶ�ļ��������ز���
	AVFrame *frame = av_frame_alloc();
	frame->format = AV_SAMPLE_FMT_FLTP;
	frame->channels = 2;
	frame->channel_layout = AV_CH_LAYOUT_STEREO;
	frame->nb_samples = 1024;//һ֡��Ƶ��ŵ���������
	av_frame_get_buffer(frame, 0);
	if (ret < 0) {
		cout << "av_frame_get_buffer error" << endl;
		getchar();
		return -1;
	}

	int readSize = frame->nb_samples * 2 * 2;
	char* pcm = new char[readSize];

	FILE* fp = fopen(infile, "rb");
	for (;;) {
		int len = fread(pcm, 1, readSize, fp);
		if (len <= 0) break;
		const uint8_t* data[1];
		data[0] = (uint8_t*)pcm;

		len = swr_convert(actx, frame->data, frame->nb_samples,
			data, frame->nb_samples);
		if (len <= 0) {
			break;
		}

		//6 ��Ƶ����
		ret = avcodec_send_frame(c, frame);
		if (ret != 0) continue;

		AVPacket pkt;
		av_init_packet(&pkt);
		ret = avcodec_receive_packet(c, &pkt);
		if (ret != 0) continue;

		//7 ��Ƶ��װ��aac�ļ�
		//pkt.stream_index = 0;
		//pkt.pts = 0;//��Ƶû��B֡�ȣ�����ֱ����0���б���
		//pkt.dts = 0;
		ret = av_interleaved_write_frame(oc, &pkt);

		cout << "[" << len << "]";
	}

	//д����Ƶ����
	av_write_trailer(oc);
	//�ر���Ƶ���io
	avio_close(oc->pb);
	//�����װ���������
	avformat_free_context(oc);

	//�رձ�����
	avcodec_close(c);
	//���������������
	avcodec_free_context(&c);

	delete[]pcm;
	pcm = NULL;

	cout << "============end===============" << endl;
	getchar();
	return 0;
}