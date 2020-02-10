#include "XPlay2.h"
#include <QFileDialog>
#include <QDebug>
#include "XDemuxThread.h"
#include <QMessageBox>

static XDemuxThread dt;

XPlay2::XPlay2(QWidget *parent)
	: QWidget(parent){
	ui.setupUi(this);

	dt.Start();//先提前开启所有线程，解封装线程，音频解码线程，视频解码线程

	startTimer(40);//启动定时器
}

//关闭应用程序，关闭释放音视频所有资源
XPlay2::~XPlay2() {
	dt.Close();
}

//定时器 滑动条显示  重载定时器方法
void XPlay2::timerEvent(QTimerEvent* e) {
	if (isSliderPress) return;//点击进度条某个位置，进度条指针暂不由定时器控制更新进度

	long long total = dt.totalMs;
	if (total > 0) {
		double pos = (double)dt.pts / (double)total;//根据音频播放时间戳来更新进度条进度
		int v = ui.playPos->maximum() * pos;
		ui.playPos->setValue(v);
	}
}

//重载:窗口尺寸变化  重载窗口尺寸事件处理
void XPlay2::resizeEvent(QResizeEvent* e) {
	ui.playPos->move(50, this->height() - 100);
	ui.playPos->resize(this->width() - 100, ui.playPos->height());
	ui.openFile->move(100, this->height() - 150);
	ui.isPlay->move(ui.openFile->x() + ui.openFile->width() + 10, ui.openFile->y());
	ui.video->resize(this->size());
}

//重载:鼠标双击事件
void XPlay2::mouseDoubleClickEvent(QMouseEvent* e) {
	if (isFullScreen()) {
		this->showNormal();
	}
	else {
		this->showFullScreen();
	}
}

void XPlay2::SetPause(bool isPause) {
	if (isPause) {
		ui.isPlay->setText(QString::fromLocal8Bit("播 放"));
	}else {
		ui.isPlay->setText(QString::fromLocal8Bit("暂 停"));
	}
}

//槽函数：响应打开视频文件播放操作
void XPlay2::openFile() {
	//选择文件
	QString name = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择视频文件"));
	//qDebug() << name;
	if (name.isEmpty()) return;
	this->setWindowTitle(name);

	if (!dt.Open(name.toLocal8Bit(), ui.video)) {
		QMessageBox::information(0, "error", "open file failed!");
		return;
	}

	SetPause(dt.isPause);
}

//槽函数：响应暂停播放和恢复播放的操作
void XPlay2::PlayOrPause() {
	bool isPause = !dt.isPause;
	SetPause(isPause);
	dt.SetPause(isPause);
}

//槽函数：响应进度条鼠标按下操作
void XPlay2::SliderPress() {
	qDebug() << "SliderPress" << endl;
	isSliderPress = true;
}

//槽函数：响应进度条鼠标释放操作，即进行seek操作
void XPlay2::SliderRelease() {
	isSliderPress = false;
	double pos = 0.0;
	pos = (double)ui.playPos->value() / (double)ui.playPos->maximum();
	dt.Seek(pos);
}