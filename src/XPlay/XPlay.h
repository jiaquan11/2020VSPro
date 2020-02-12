#pragma once

#include <QtWidgets/QWidget>
#include "ui_XPlay.h"

class XPlay : public QWidget
{
	Q_OBJECT

public:
	XPlay(QWidget *parent = Q_NULLPTR);
	~XPlay();

	void resizeEvent(QResizeEvent* e);
	void timerEvent(QTimerEvent* e);

public slots:
	void open();
	void play();
	void sliderPress();
	void sliderRelease();

private:
	Ui::XPlayClass ui;
};
