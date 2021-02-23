﻿#include <QtCore/QCoreApplication>
#include "XMediaEncode.h"
#include "XRtmp.h"
#include "XAudioRecord.h"
#include "XVideoCapture.h"
#include "XFilter.h"
#include "XController.h"

#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	XController::Get()->Stop();
	XController::Get()->camIndex = 0;
	//nginx - rtmp 直播服务器推流url
	XController::Get()->outUrl = "rtmp://192.168.174.129/live/"; 
	XController::Get()->Start();

	XController::Get()->wait();

	return a.exec();
}