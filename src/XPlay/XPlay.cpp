#include "XPlay.h"
#include <QFileDialog>
#include <QMessageBox>
#include "XFFmpeg.h"
#include "XAudioPlay.h"

static bool isPressSlider = false;
static bool isPlay = true;
#define PAUSE "QPushButton\
{border-image: url\
(:/XPlay/Resources/pause_normal.png);}"

#define PLAY "QPushButton\
{border-image: url\
(:/XPlay/Resources/play_normal.png);}"

 XPlay::XPlay(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	startTimer(40);
}

XPlay::~XPlay() {

}

void XPlay::open() {
	QString name = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择视频文件"));
	if (name.isEmpty())
		return;

	this->setWindowTitle(name);
	int totalMs = XFFmpeg::Get()->Open(name.toLocal8Bit());
	if (totalMs <= 0) {
		QMessageBox::information(this, "err", "file open failed!");
		return;
	}

	XAudioPlay::Get()->sampleRate = XFFmpeg::Get()->sampleRate;
	XAudioPlay::Get()->channel = XFFmpeg::Get()->channel;
	XAudioPlay::Get()->Start();

	char buf[1024] = { 0 };
	int min = (totalMs / 1000) / 60;//分
	int sec = (totalMs / 1000) % 60;//秒
	sprintf(buf, "%03d:%02d", min, sec);
	ui.totalTime->setText(buf);

	isPlay = false;
	play();
}

void XPlay::play() {
	isPlay = !isPlay;
	XFFmpeg::Get()->isPlay = isPlay;

	if (isPlay) {
		//pause
		ui.playBtn->setStyleSheet(PAUSE);
	}
	else {
		ui.playBtn->setStyleSheet(PLAY);
	}
}

void XPlay::sliderPress() {
	isPressSlider = true;
}

void XPlay::sliderRelease() {
	isPressSlider = false;
	float pos = 0;
	pos = (float)ui.playSlider->value() / (float)(ui.playSlider->maximum() + 1);
	XFFmpeg::Get()->Seek(pos);
}

//界面控件变化调整
void XPlay::resizeEvent(QResizeEvent* e) {
	ui.openGLWidget->resize(size());

	ui.playBtn->move(this->width() / 2 + 50, this->height() - 80);
	ui.openBtn->move(this->width() / 2 - 50, this->height() - 80);
	ui.playSlider->move(25, this->height() - 120);
	ui.playSlider->resize(this->width() - 50, ui.playSlider->height());
	ui.playTime->move(25, ui.playBtn->y());
	ui.sp->move(ui.playTime->x() + ui.playTime->width() + 5, ui.playTime->y());
	ui.totalTime->move(150, ui.playBtn->y());

}

//视频播放过程中，进度条按照定时器设置进行刷新，以播放时间戳的位置进行刷新
void XPlay::timerEvent(QTimerEvent* e) {
	int min = (XFFmpeg::Get()->pts / 1000) / 60;
	int sec = (XFFmpeg::Get()->pts / 1000) % 60;
	char buf[1024] = { 0 };
	sprintf(buf, "%03d:%02d", min, sec);
	ui.playTime->setText(buf);

	if (XFFmpeg::Get()->totalMs > 0) {
		float rate = (float)XFFmpeg::Get()->pts / (float)XFFmpeg::Get()->totalMs;
		if (!isPressSlider) {//按下进度条时，进度条不刷新
			ui.playSlider->setValue(rate * 1000);
		}
	}
}