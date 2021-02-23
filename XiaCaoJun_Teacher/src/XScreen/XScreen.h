#pragma once

#include <QtWidgets/QWidget>
#include "ui_XScreen.h"

class XScreen : public QWidget
{
	Q_OBJECT

public:
	XScreen(QWidget *parent = Q_NULLPTR);
	void timerEvent(QTimerEvent* e);

public slots:
	void Record();

private:
	Ui::XScreenClass ui;
};
