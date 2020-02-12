#pragma once

#include <QSlider>

class XSlider : public QSlider
{
	Q_OBJECT

public:
	XSlider(QWidget *parent);
	~XSlider();

	void mousePressEvent(QMouseEvent *e);
};
