#include "XPlay2.h"
#include <QtWidgets/QApplication>
#include "XDemux.h"
#include "XDecode.h"
#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{
	//≤‚ ‘XDemux
	char* url = "testvideo1.mp4";
	//char* url = "http://devimages.apple.com.edgekey.net/streaming/examples/bipbop_4x3/gear2/prog_index.m3u8";

	XDemux demux;
	cout << "demux.Open = " << demux.Open(url) << endl;
	demux.Read();
	demux.Clear();
	demux.Close();
	cout << "demux.Open = " << demux.Open(url) << endl;
	cout << "CopyVPara = " << demux.CopyVPara() << endl;
	cout << "CopyAPara = " << demux.CopyAPara() << endl;
	//cout << "seek= " << demux.Seek(0.9) << endl; 

	XDecode vdecode;
	cout << "vdecode.Open() =" << vdecode.Open(demux.CopyVPara()) << endl;
	//vdecode.Clear();
	//vdecode.Close();
	XDecode adecode;
	cout << "adecode.Open() =" << adecode.Open(demux.CopyAPara()) << endl;

	for (;;){
		AVPacket* pkt = demux.Read();
		if (demux.IsAudio(pkt)){
			adecode.Send(pkt);
			AVFrame* frame = adecode.Recv();
		}else {
			vdecode.Send(pkt);
			AVFrame* frame = vdecode.Recv();
		}

		if (!pkt) break;
	}

	QApplication a(argc, argv);
	XPlay2 w;
	w.show();
	return a.exec();
}
