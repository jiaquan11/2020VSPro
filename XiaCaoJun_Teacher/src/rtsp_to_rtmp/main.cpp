#include <iostream>
using namespace std;

extern "C" {
#include "libavformat/avformat.h"
#include <libavutil/time.h>
}
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "avcodec.lib")

int XError(int errNum) {
	char buf[1024] = { 0 };
	av_strerror(errNum, buf, sizeof(buf));
	getchar();
	return -1;
}

static double r2d(AVRational r) {
	return r.num == 0 || r.den == 0 ? 0. : (double)r.num / (double)r.den;
}

int main(int argc, char* argv[]) {
	cout << "file to rtmp test" << endl;
	//char* inUrl = "demo_video.flv";
	char* inUrl = "rtsp://test:test123456@192.168.1.64";
	char* outUrl = "rtmp://192.168.174.129/live/";//ָ������·��

	//��ʼ�����з�װ�ͽ��װ��flv mp4 mov mp3
	av_register_all();
	//��ʼ�������
	avformat_network_init();

	//////////////////////////////////////////////////////////
	////������
	//1..���ļ������װ
	//�����װ������
	AVFormatContext* ictx = NULL;
	//��rtsp��������ļ�ͷ
	//����rtspЭ�������ʱ
	AVDictionary* opts = NULL;
	char key[] = "max_delay";
	char val[] = "500";
	av_dict_set(&opts, key, val, 0);
	char key2[] = "rtsp_transport";
	char val2[] = "tcp";
	av_dict_set(&opts, key2, val2, 0);
	int ret = avformat_open_input(&ictx, inUrl, 0, &opts);
	if (ret != 0) {
		return XError(ret);
	}
	cout << "open file " << inUrl << " Success!" << endl;

	//��ȡ����Ƶ����Ϣ
	ret = avformat_find_stream_info(ictx, 0);
	if (ret != 0) {
		return XError(ret);
	}
	
	av_dump_format(ictx, 0, inUrl, 0);
	////////////////////////////////////////////////////////////
	/////�����
	//���������������
	AVFormatContext *octx = NULL;
	ret = avformat_alloc_output_context2(&octx, 0, "flv", outUrl);
	if (!octx) {
		return XError(ret);
	}
	cout << "octx create success!" << endl;

	//���������
	//���������AVStream
	for (int i = 0; i < ictx->nb_streams; i++) {
		//���������
		AVStream* out = avformat_new_stream(octx, NULL);
		if (!out) {
			return XError(0);
		}
		//����������Ϣ,ͬ��mp4
		//ret = avcodec_copy_context(out->codec, ictx->streams[i]->codec);
		ret = avcodec_parameters_copy(out->codecpar, ictx->streams[i]->codecpar);
		out->codec->codec_tag = 0;
	}

	av_dump_format(octx, 0, outUrl, 1);

	//rtmp����
	//��io
	ret = avio_open(&octx->pb, outUrl, AVIO_FLAG_WRITE);
	if (!octx->pb) {
		return XError(ret);
	}

	//д��ͷ��Ϣ
	ret = avformat_write_header(octx, 0);
	if (ret < 0) {
		return XError(ret);
	}
	cout << "avformat_write_header " << ret << endl;

	//����ÿһ֡����
	AVPacket pkt;
	long long startTime = av_gettime();
	for (;;) {
		ret = av_read_frame(ictx, &pkt);
		if ((ret != 0) || (pkt.size <= 0)) {
			continue;
		}
		//����ת��pts dts
		AVRational itime = ictx->streams[pkt.stream_index]->time_base;
		AVRational otime = octx->streams[pkt.stream_index]->time_base;
		pkt.pts = av_rescale_q_rnd(pkt.pts, itime, otime, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
		pkt.dts = av_rescale_q_rnd(pkt.dts, itime, otime, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
		pkt.duration = av_rescale_q_rnd(pkt.duration, itime, otime, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
		pkt.pos = -1;

		/*
		av_interleaved_write_frame�ڲ����Զ��������򣬼�����pkt�ڴ棬
		�����ⲿ���ͷ�pkt
		*/
		/*��Ƶ֡�����ٶȿ���
		���Ʋ��ԣ��Զ�ȡ��packet pts�͵�ǰϵͳʱ�Ӽ��������м�ʱ�Ա�
		��������İ���dts���ڼ�ʱ����ʱ�䣬����ʱ���͡���֤���յ�ǰ��ʱ��
		ʱ�ӽ�������
		*/

		//����rtsp�����������������Ϊrtsp���Ѿ�������������
		//if (ictx->streams[pkt.stream_index]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
		//	AVRational tb = ictx->streams[pkt.stream_index]->time_base;
		//	long long now = av_gettime() - startTime;
		//	long long dts = 0;
		//	dts = pkt.dts *(1000*1000*r2d(tb));//΢��
		//	pkt.pts = pkt.pts *(1000 * 1000 * r2d(tb));//΢��
		//	cout << "pts: " << pkt.pts/1000 << " dts: " << dts/1000<<"| ";
		//	if (dts > now) {
		//		av_usleep(dts-now);
		//	}
		//}


		ret = av_interleaved_write_frame(octx, &pkt);//����
		if (ret < 0) {
			//return XError(ret);
			XError(ret);//����rtsp�����ܿ�ʼ�����������ֱ�ӷ��ش���
		}

		//�°汾ffmpeg�Ƽ�ʹ��av_packet_unref�ͷ�pkt�ڴ棬av_free_packet�ѱ�����
		//av_packet_unref(&pkt);
	}

	getchar();

	return 0;
}