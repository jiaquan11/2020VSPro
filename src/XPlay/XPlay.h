#pragma once

#include <QtWidgets/QWidget>
#include "ui_XPlay.h"

class XPlay : public QWidget
{
	Q_OBJECT

public:
	XPlay(QWidget *parent = Q_NULLPTR);

private:
	Ui::XPlayClass ui;
};
