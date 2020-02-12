#include "XPlay.h"
#include <QtWidgets/QApplication>
#include "XFFmpeg.h"
#include <QAudioOutput>

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
	QApplication a(argc, argv);
	XPlay w;
	w.show();
	return a.exec();
}
