#pragma once

#include <QtWidgets/QWidget>
#include "ui_XRtmpStreamer.h"

class XRtmpStreamer : public QWidget
{
	Q_OBJECT

public:
	XRtmpStreamer(QWidget *parent = Q_NULLPTR);

public slots:
	void startStream();

private:
	Ui::XRtmpStreamerClass ui;
};
