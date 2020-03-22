#include "XVideoUI.h"
#include <QFileDialog>
#include <QMessageBox>
#include <string>
#include "XVideoThread.h"
#include "XFilter.h"
#include "XAudio.h"
#include <iostream>
#include <QFile>
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
using namespace std;
using namespace cv;

static bool pressSlider = false;
static bool isExport = false;
static bool isColor = true;
static bool isMark = false;
static bool isBlend = false;
static bool isMerge = false;

XVideoUI::XVideoUI(QWidget *parent)
	: QWidget(parent){
	ui.setupUi(this);
	setWindowFlags(Qt::FramelessWindowHint);

	qRegisterMetaType<cv::Mat>("cv::Mat");//注册Qt不支持的类型
	//原视频显示信号
	QObject::connect(XVideoThread::Get(),//建立信号和槽函数的连接
		SIGNAL(ViewImage1(cv::Mat)),
		ui.srcvideo1,
		SLOT(setImage(cv::Mat)));

	//原视频2显示信号
	QObject::connect(XVideoThread::Get(),//建立信号和槽函数的连接
		SIGNAL(ViewImage2(cv::Mat)),
		ui.srcvideo2,
		SLOT(setImage(cv::Mat)));

	//输出视频显示信号
	QObject::connect(XVideoThread::Get(),//建立信号和槽函数的连接
		SIGNAL(ViewDes(cv::Mat)),
		ui.desVideo,
		SLOT(setImage(cv::Mat)));

	//导出视频结束
	QObject::connect(XVideoThread::Get(),//建立信号和槽函数的连接
		SIGNAL(SaveEnd()),
		this,
		SLOT(ExportEnd()));

	pause();

	startTimer(40);
}

void XVideoUI::open() {
	QString name = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("请选择视频文件"));
	if (name.isEmpty()) return;

	std::string file = name.toLocal8Bit().data();
	if (!XVideoThread::Get()->open(file)) {
		//QMessageBox::information(this, "error", name+" open failed!");
		cout << "open: " << file <<" failed"<< endl;
		return;
	}

	play();//默认打开就播放

	cout << "open: " << file << " success!" << endl;
}

void XVideoUI::play() {
	ui.pauseBtn->show();
	ui.pauseBtn->setGeometry(ui.playBtn->geometry());
	XVideoThread::Get()->play();
	ui.playBtn->hide();
}

void XVideoUI::pause() {
	ui.playBtn->show();
	//ui.playBtn->setGeometry(ui.playBtn->geometry());
	ui.pauseBtn->hide();
	XVideoThread::Get()->pause();
}

void XVideoUI::sliderPress() {
	pressSlider = true;
}

void XVideoUI::sliderRelease() {
	pressSlider = false;
}

//滑动条拖动
void XVideoUI::setPos(int pos) {
	XVideoThread::Get()->seek((double)(pos / 1000.));
}

void XVideoUI::left(int pos) {
	XVideoThread::Get()->setBegin((double)(pos / 1000.));
	setPos(pos);
}

void XVideoUI::right(int pos) {
	XVideoThread::Get()->setEnd((double)(pos / 1000.));
}

//设置过滤器
void XVideoUI::set() {
	XFilter::Get()->Clear();

	isColor = true;

	//视频图像裁剪
	bool isClip = false;
	double cx = ui.cx->value();
	double cy = ui.cy->value();
	double cw = ui.cw->value();
	double ch = ui.ch->value();
	if (cx + cy + cw + ch > 0.00001) {
		isClip = true;
		XFilter::Get()->Add(XTask{ XTASK_CLIP,
		{cx, cy, cw, ch} });

		//裁剪后以原尺寸缩放输出
		double w = XVideoThread::Get()->width;
		double h = XVideoThread::Get()->height;
		XFilter::Get()->Add(XTask{ XTASK_RESIZE , { w,h } });
	}

	//图像金字塔
	bool isPy = false;
	int down = ui.pyDown->value();
	int up = ui.pyUp->value();
	if (!isClip && (down > 0)) {
		isPy = true;
		XFilter::Get()->Add(XTask{ XTASK_PYDOWN, {(double)down } });
		int w = XVideoThread::Get()->width;
		int h = XVideoThread::Get()->height;
		for (int i = 0; i < down; i++) {
			w = w / 2;
			h = h / 2;
		}
		ui.width->setValue(w);
		ui.height->setValue(h);
	}
	if (!isClip && (up > 0)) {
		isPy = true;
		XFilter::Get()->Add(XTask{ XTASK_PYUP,{ (double)up } });
		int w = XVideoThread::Get()->width;
		int h = XVideoThread::Get()->height;
		for (int i = 0; i < up; i++) {
			w = w * 2;
			h = h * 2;
		}
		ui.width->setValue(w);
		ui.height->setValue(h);
	}

	//调整图像尺寸
	double w = ui.width->value();
	double h = ui.height->value();
	if (!isMerge && !isClip && !isPy && (ui.width->value() > 0) && (ui.height->value() > 0)) {
		XFilter::Get()->Add(XTask{ XTASK_RESIZE, {w, h} });
	}

	//对比度和亮度
	if (ui.bright->value() > 0 ||
		ui.contrast->value() > 1) {
		XFilter::Get()->Add(XTask{ XTASK_GAIN,
		{(double)ui.contrast->value(), (double)ui.bright->value()} });
	}

	//灰度图
	if (ui.color->currentIndex() == 1) {
		XFilter::Get()->Add(XTask{ XTASK_GRAY });
		isColor = false;
	}

	//图像旋转 1:90 2:180 3:270
	if (ui.rotate->currentIndex() == 1) {
		XFilter::Get()->Add(XTask{ XTASK_ROTATE90 });
	}
	else if (ui.rotate->currentIndex() == 2) {
		XFilter::Get()->Add(XTask{ XTASK_ROTATE180 });
	}
	else if (ui.rotate->currentIndex() == 3) {
		XFilter::Get()->Add(XTask{ XTASK_ROTATE270 });
	}

	//图像镜像
	if (ui.flip->currentIndex() == 1) {
		XFilter::Get()->Add(XTask{ XTASK_FLIPX });
	}
	else if (ui.flip->currentIndex() == 2) {
		XFilter::Get()->Add(XTask{ XTASK_FLIPY });
	}
	else if (ui.flip->currentIndex() == 3) {
		XFilter::Get()->Add(XTask{ XTASK_FLIPXY });
	}

	//水印
	if (isMark) {
		double x = ui.mx->value();
		double y = ui.my->value();
		double a = ui.ma->value();
		XFilter::Get()->Add(XTask{ XTASK_MASK, {x,y,a} });
	}

	//融合
	if (isBlend) {
		double a = ui.ba->value();
		XFilter::Get()->Add(XTask{ XTASK_BLEND, {a} });
	}

	//合并
	if (isMerge) {
		double h2 = XVideoThread::Get()->height2;
		double h1 = XVideoThread::Get()->height;
		int w = XVideoThread::Get()->width2*(h2 / h1);
		XFilter::Get()->Add(XTask{ XTASK_MERGE });
		ui.width->setValue(XVideoThread::Get()->width + w);
		ui.height->setValue(h1);
	}
}

void XVideoUI::timerEvent(QTimerEvent* e) {
	if (pressSlider) return;

	double pos = XVideoThread::Get()->getPos();
	ui.playSlider->setValue(pos*1000);
}

//导出视频
void XVideoUI::Export() {
	if (isExport) {
		//停止导出
		XVideoThread::Get()->stopSave();
		isExport = false;
		ui.exportButton->setText("Start Export");
		return;
	}

	//开始导出
	QString name = QFileDialog::getSaveFileName(
		this, "save", "outcpp001.avi");
	if (name.isEmpty()) return;
	std::string filename = name.toLocal8Bit().data();
	int w = ui.width->value();
	int h = ui.height->value();
	if (XVideoThread::Get()->startSave(filename, w, h, isColor)) {
		isExport = true;
		ui.exportButton->setText("Stop Export");
	}
}

//导出结束
void XVideoUI::ExportEnd() {
	isExport = false;
	ui.exportButton->setText("Start Export");

	//处理音频
	string src = XVideoThread::Get()->srcFile1;
	string des = XVideoThread::Get()->destFile;

	int ss = 0;
	int t = 0;
	ss = XVideoThread::Get()->totalMs * ((double)ui.left->value() / 1000);
	int end = XVideoThread::Get()->totalMs * ((double)ui.right->value() / 1000);
	t = end - ss;

	XAudio::Get()->ExportA(src, src + ".mp3");
	string tmp = des + ".avi";
	QFile::remove(tmp.c_str());
	QFile::rename(des.c_str(), tmp.c_str());
	XAudio::Get()->Merge(tmp, src + ".mp3", des);
}

//水印
void XVideoUI::mark() {
	isMark = false;
	isBlend = false;
	isMerge = false;

	QString name = QFileDialog::getOpenFileName(this, "select image:");
	if (name.isEmpty()) {
		return;
	}
	std::string file = name.toLocal8Bit().data();
	cv::Mat mark = imread(file);
	if (mark.empty()) return;

	XVideoThread::Get()->setMark(mark);
	isMark = true;
}

//融合
void XVideoUI::blend() {
	isMark = false;
	isBlend = false;
	isMerge = false;

	QString name = QFileDialog::getOpenFileName(this, "select video:");
	if (name.isEmpty()) {
		return;
	}
	std::string file = name.toLocal8Bit().data();
	isBlend = XVideoThread::Get()->open2(file);
	cout << "isBlend: " << isBlend << endl;
}

//合并
void XVideoUI::merge() {
	isMark = false;
	isBlend = false;
	isMerge = false;

	QString name = QFileDialog::getOpenFileName(this, "select video:");
	if (name.isEmpty()) {
		return;
	}
	std::string file = name.toLocal8Bit().data();
	isMerge = XVideoThread::Get()->open2(file);
	cout << "isMerge: " << isMerge << endl;
}