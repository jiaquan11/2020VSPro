#include <iostream>
using namespace std;

extern "C" {
	#include <libavformat/avformat.h>
}

#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib,"avutil.lib")

int main() {
	char infile[] = "testvideo1.mp4";
	char outfile[] = "out.mov";

	av_register_all();

	//1.open input file
	AVFormatContext* ic = NULL;
	avformat_open_input(&ic, infile, 0, 0);
	if (!ic){
		cerr << "avformat_open_input failed!" << endl;
		getchar();
		return -1;
	}

	cout << "open " << infile << " success!" << endl;

	//2.create output context
	AVFormatContext* oc = NULL;
	avformat_alloc_output_context2(&oc, NULL, NULL, outfile);
	if (!oc) {
		cerr << "avformat_alloc_output_context2 failed!" << endl;
		getchar();
		return -1;
	}

	//3.add the stream
	AVStream* videoStream = avformat_new_stream(oc, NULL);
	AVStream* audioStream = avformat_new_stream(oc, NULL);

	//4.copy para
	avcodec_parameters_copy(videoStream->codecpar, ic->streams[0]->codecpar);
	avcodec_parameters_copy(audioStream->codecpar, ic->streams[1]->codecpar);

	videoStream->codecpar->codec_tag = 0;
	audioStream->codecpar->codec_tag = 0;

	av_dump_format(ic, 0, infile, 0);
	cout << "===================================" << endl;
	av_dump_format(oc, 0, outfile, 1);

	//5 open out file io, write head
	int ret = avio_open(&oc->pb, outfile, AVIO_FLAG_WRITE);
	if (ret < 0) {
		cerr << "avio open failed!" << endl;
		getchar();
		return -1;
	}
	ret = avformat_write_header(oc, NULL);
	if (ret < 0) {
		cerr << "avformat_write_header failed!" << endl;
		getchar();
	}

	AVPacket pkt;
	for (;;) {
		int ret = av_read_frame(ic, &pkt);
		if (ret < 0) {
			break;
		}

		/*
		这里时间戳换算很重要
		表示将原文件以time_base为基准的数据包pts,转换为新文件中以新的time_base为基准的pts,
		并才能写入文件中
		*/
		pkt.pts = av_rescale_q_rnd(pkt.pts,
			ic->streams[pkt.stream_index]->time_base,
			oc->streams[pkt.stream_index]->time_base,
			(AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
		pkt.dts = av_rescale_q_rnd(pkt.dts,
			ic->streams[pkt.stream_index]->time_base,
			oc->streams[pkt.stream_index]->time_base,
			(AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));

		pkt.pos = -1;//数据包的位置，设为-1表示写入文件后由新文件重新赋值

		pkt.duration = av_rescale_q_rnd(pkt.duration,
			ic->streams[pkt.stream_index]->time_base,
			oc->streams[pkt.stream_index]->time_base,
			(AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));//表示已写入的时长


		av_write_frame(oc, &pkt);
		av_packet_unref(&pkt);
		cout << ".";
	}

	av_write_trailer(oc);
	avio_close(oc->pb);
	cout << "==================end=====================" << endl;

	getchar();
	return 0;
}