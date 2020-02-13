#include "XSlider.h"
#include <QMouseEvent>

XSlider::XSlider(QWidget *parent)
	: QSlider(parent){

}

XSlider::~XSlider(){

}

//滑动条鼠标按下监听事件处理，更新滑动条位置
void XSlider::mousePressEvent(QMouseEvent *e){
	int value = ((float)e->pos().x() / (float)this->width())*this->maximum() + 1;
	this->setValue(value);
	QSlider::mousePressEvent(e);
}
