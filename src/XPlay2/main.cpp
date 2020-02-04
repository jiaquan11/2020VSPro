#include "XPlay2.h"
#include <QtWidgets/QApplication>
#include "XDemux.h"
#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{
	//≤‚ ‘XDemux
	//char* url = "testvideo1.mp4";
	char* url = "http://devimages.apple.com.edgekey.net/streaming/examples/bipbop_4x3/gear2/prog_index.m3u8";

	XDemux demux;
	cout << "demux.Open = " << demux.Open(url) << endl;


	QApplication a(argc, argv);
	XPlay2 w;
	w.show();
	return a.exec();
}
