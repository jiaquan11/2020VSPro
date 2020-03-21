#include "XVideoUI.h"
#include <QFileDialog>
#include <QMessageBox>
#include <string>
#include "XVideoThread.h"
#include "XFilter.h"
#include <iostream>
using namespace std;

static bool pressSlider = false;
static bool isExport = false;

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

	cout << "open: " << file << " success!" << endl;
}

void XVideoUI::sliderPress() {
	pressSlider = true;
}

void XVideoUI::sliderRelease() {
	pressSlider = false;
}

//滑动条拖动
void XVideoUI::setPos(int pos) {
	XVideoThread::Get()->seek((double)(pos / 1000));
}

//设置过滤器
void XVideoUI::set() {
	XFilter::Get()->Clear();

	//对比度和亮度
	if (ui.bright->value() > 0 ||
		ui.contrast->value() > 1) {
		XFilter::Get()->Add(XTask{ XTASK_GAIN,
		{(double)ui.contrast->value(), (double)ui.bright->value()} });
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
	if (XVideoThread::Get()->startSave(filename)) {
		isExport = true;
		ui.exportButton->setText("Stop Export");
	}
}

//导出结束
void XVideoUI::ExportEnd() {
	isExport = false;
	ui.exportButton->setText("Start Export");
}