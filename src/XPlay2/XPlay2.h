#pragma once

#include <QtWidgets/QWidget>
#include "ui_XPlay2.h"

class XPlay2 : public QWidget
{
	Q_OBJECT

public:
	XPlay2(QWidget *parent = Q_NULLPTR);
	~XPlay2();

	//定时器 滑动条显示
	void timerEvent(QTimerEvent* e);

public slots:
	void openFile();

private:
	Ui::XPlay2Class ui;
};
