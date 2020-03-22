#include "XVideoUI.h"
#include <QtWidgets/QApplication>
#include "XAudio.h"

int main(int argc, char *argv[])
{
	//XAudio::Get()->ExportA("testvideo1.mp4", "test.mp3");

	QApplication a(argc, argv);
	XVideoUI w;
	w.show();
	return a.exec();
}
