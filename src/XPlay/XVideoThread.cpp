#include "XVideoThread.h"
#include "XFFmpeg.h"
#include "XAudioPlay.h"
#include <list>
using namespace std;

bool isExit = false;
static list<AVPacket> videos;
static int apts = -1;

XVideoThread::XVideoThread(){

}

XVideoThread::~XVideoThread(){
	isExit = true;
	wait();
}

void XVideoThread::run() {
	char out[10000] = {0};

	while (!isExit){
		if (!XFFmpeg::Get()->isPlay) {
			msleep(10);
			continue;
		}
		
		while (videos.size() > 0) {
			AVPacket pack = videos.front();
			int pts = XFFmpeg::Get()->GetPts(&pack);
			if (pts > apts) {
				break;
			}
			XFFmpeg::Get()->Decode(&pack);
			av_packet_unref(&pack);
			videos.pop_front();
		}

		int free = XAudioPlay::Get()->GetFree();
		if (free < 4096) {
			msleep(1);
			continue;
		}

		AVPacket pkt = XFFmpeg::Get()->Read();
		if (pkt.size <= 0) {
			msleep(10);
			continue;
		}

		//音频流
		if (pkt.stream_index == XFFmpeg::Get()->audioStream) {
			apts = XFFmpeg::Get()->Decode(&pkt);
			av_packet_unref(&pkt);
			int len = XFFmpeg::Get()->ToPCM(out);
			XAudioPlay::Get()->Write(out, len);
			continue;
		}

		//非字幕流
		if (pkt.stream_index != XFFmpeg::Get()->videoStream) {
			av_packet_unref(&pkt);
			continue;
		}

		videos.push_back(pkt);
	}
}
