#include "XPlay2.h"
#include <QtWidgets/QApplication>
#include "XDemux.h"
#include "XDecode.h"
#include "XResample.h"
#include <QThread>
#include "XAudioPlay.h"
#include "XAudioThread.h"
#include "XVideoThread.h"
#include "IVideoCall.h"
#include <iostream>
using namespace std;

class TestThread : public QThread {
public:
	XAudioThread at;
	XVideoThread vt;

	void Init() {
		char* url = "testvideo1.mp4";
		cout << "demux.Open = " << demux.Open(url) << endl;
		//cout << "vdecode.Open() =" << vdecode.Open(demux.CopyVPara()) << endl; 
		//cout << "adecode.Open() =" << adecode.Open(demux.CopyAPara()) << endl;

		//cout << "resample.Open = " << resample.Open(demux.CopyAPara()) << endl;
		//XAudioPlay::Get()->channels = demux.channels;
		//XAudioPlay::Get()->sampleRate = demux.sampleRate;

		//cout << "XAudioPlay::Get()->Open() = " << XAudioPlay::Get()->Open() << endl;

		cout << "at.Open() = " << at.Open(demux.CopyAPara(), demux.sampleRate, demux.channels) << endl;
		vt.Open(demux.CopyVPara(), video, demux.width, demux.height);

		at.start();
		vt.start();
	}

	unsigned char* pcm = new unsigned char[1024 * 1024];
	void run() {
		for (;;) {
			AVPacket* pkt = demux.Read();
			if (demux.IsAudio(pkt)) {
				at.Push(pkt);

				/*adecode.Send(pkt);
				AVFrame* frame = adecode.Recv();
				int len = resample.Resample(frame, pcm);
				cout<<"Resample: "<<len<<" ";
				while (len > 0) {
					if (XAudioPlay::Get()->GetFree() >= len){
						XAudioPlay::Get()->Write(pcm, len);
						break;
					}
					msleep(1);
				}*/
			}else {
				vt.Push(pkt);
				//vdecode.Send(pkt);
				//AVFrame* frame = vdecode.Recv();
				//video->Repaint(frame);
				//msleep(33);
			}

			if (!pkt) break;
		}
	}

	//²âÊÔXDemux
	XDemux demux;
	//½âÂë²âÊÔ
	//XDecode vdecode;
	//XDecode adecode;
	//XResample resample;

	XVideoWidget *video = NULL;
};

int main(int argc, char *argv[])
{
	TestThread tt;

	QApplication a(argc, argv);
	XPlay2 w;
	w.show();

	//³õÊ¼»¯gl´°¿Ú
	//w.ui.video->Init(tt.demux.width, tt.demux.height);
	tt.video = w.ui.video;
	tt.Init();
	tt.start();

	return a.exec();
}
