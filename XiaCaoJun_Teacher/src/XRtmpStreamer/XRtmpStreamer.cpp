#include "XRtmpStreamer.h"
#include <iostream>
#include "XController.h"

using namespace std;

static bool isStream = false;

XRtmpStreamer::XRtmpStreamer(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

void XRtmpStreamer::startStream() {
	if (isStream) {
		isStream = false;
		ui.startBtn->setText(QString::fromLocal8Bit("¿ªÊ¼"));
		XController::Get()->Stop();
	}
	else {
		isStream = true;
		ui.startBtn->setText(QString::fromLocal8Bit("Í£Ö¹"));
		QString url = ui.inUrl->text();
		bool ok = false;
		int camIndex = url.toInt(&ok);
		if (!ok) {
			XController::Get()->inUrl = url.toStdString();
		}
		else {
			XController::Get()->camIndex = camIndex;
		}
		XController::Get()->outUrl = ui.outUrl->text().toStdString();
		XController::Get()->Set("b", (ui.faceLevel->currentIndex() + 1) * 3);
		XController::Get()->Start();
	}
}