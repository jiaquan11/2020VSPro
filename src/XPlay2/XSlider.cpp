#include "XSlider.h"

XSlider::XSlider(QWidget *parent)
	: QSlider(parent){

}

XSlider::~XSlider(){

}

//重载鼠标点击进度条事件处理
void XSlider::mousePressEvent(QMouseEvent* e) {
	double pos = (double)e->pos().x() / (double)width();
	setValue(pos*this->maximum());//更新进度条位置
	//原有事件处理
	//QSlider::mousePressEvent(e);
	QSlider::sliderReleased();
}
