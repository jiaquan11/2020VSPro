#pragma once

#include <QtWidgets/QWidget>
#include "ui_TestQtMat.h"

class TestQtMat : public QWidget
{
	Q_OBJECT

public:
	TestQtMat(QWidget *parent = Q_NULLPTR);

private:
	Ui::TestQtMatClass ui;
};
