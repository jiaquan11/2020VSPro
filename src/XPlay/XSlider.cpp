#include "XSlider.h"
#include <QMouseEvent>

XSlider::XSlider(QWidget *parent)
	: QSlider(parent){

}

XSlider::~XSlider(){

}

void XSlider::mousePressEvent(QMouseEvent *e){
	int value = ((float)e->pos().x() / (float)this->width())*this->maximum() + 1;
	this->setValue(value);
	QSlider::mousePressEvent(e);
}
