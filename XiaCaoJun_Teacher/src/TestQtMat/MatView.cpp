#include "MatView.h"
#include <QImage>
#include <QPainter>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
using namespace cv;
#pragma comment(lib, "opencv_world420d.lib")

MatView::MatView(QWidget* p) : QOpenGLWidget(p){

}

MatView::~MatView(){

}

void MatView::paintEvent(QPaintEvent *e) {
	Mat src = imread("lena.jpg");//opencv读取的图像格式是BGR
	cvtColor(src, src, COLOR_BGR2RGB);
	QImage img(src.data, src.cols, src.rows, QImage::Format_RGB888);//QT渲染的是RGB格式
	QPainter painter;
	painter.begin(this);
	painter.drawImage(QPoint(0, 0), img);
	painter.end();
}