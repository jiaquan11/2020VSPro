#include "XPlay2.h"
#include <QtWidgets/QApplication>
#include "XDemux.h"
#include "XDecode.h"
#include "XResample.h"
#include <QThread>

#include <iostream>
using namespace std;

class TestThread : public QThread {
public:
	void Init() {
		char* url = "testvideo1.mp4";
		cout << "demux.Open = " << demux.Open(url) << endl;
		cout << "vdecode.Open() =" << vdecode.Open(demux.CopyVPara()) << endl; 
		cout << "adecode.Open() =" << adecode.Open(demux.CopyAPara()) << endl;

		cout << "resample.Open = " << resample.Open(demux.CopyAPara()) << endl;
	}

	unsigned char* pcm = new unsigned char[1024 * 1024];
	void run() {
		for (;;) {
			AVPacket* pkt = demux.Read();
			if (demux.IsAudio(pkt)) {
				adecode.Send(pkt);
				AVFrame* frame = adecode.Recv();
				cout<<"Resample: "<<resample.Resample(frame, pcm)<<" ";
			}else {
				vdecode.Send(pkt);
				AVFrame* frame = vdecode.Recv();
				video->Repaint(frame);
				msleep(33);
			}

			if (!pkt) break;
		}
	}

	//²âÊÔXDemux
	XDemux demux;
	//½âÂë²âÊÔ
	XDecode vdecode;
	XDecode adecode;
	XResample resample;

	XVideoWidget *video;
};

int main(int argc, char *argv[])
{
	TestThread tt;
	tt.Init();

	QApplication a(argc, argv);
	XPlay2 w;
	w.show();

	//³õÊ¼»¯gl´°¿Ú
	w.ui.video->Init(tt.demux.width, tt.demux.height);
	tt.video = w.ui.video;
	tt.start();

	return a.exec();
}
