#include "XPlay2.h"
#include <QFileDialog>
#include <QDebug>
#include "XDemuxThread.h"
#include <QMessageBox>

static XDemuxThread dt;

XPlay2::XPlay2(QWidget *parent)
	: QWidget(parent){
	ui.setupUi(this);

	dt.Start();

	startTimer(40);//������ʱ��
}

XPlay2::~XPlay2() {
	dt.Close();
}

//��ʱ�� ��������ʾ  ���ض�ʱ������
void XPlay2::timerEvent(QTimerEvent* e) {
	if (isSliderPress) return;

	long long total = dt.totalMs;
	if (total > 0) {
		double pos = (double)dt.pts / (double)total;
		int v = ui.playPos->maximum() * pos;
		ui.playPos->setValue(v);
	}
}

//���ڳߴ�仯  ���ش��ڳߴ��¼�����
void XPlay2::resizeEvent(QResizeEvent* e) {
	ui.playPos->move(50, this->height() - 100);
	ui.playPos->resize(this->width() - 100, ui.playPos->height());
	ui.openFile->move(100, this->height() - 150);
	ui.isPlay->move(ui.openFile->x() + ui.openFile->width() + 10, ui.openFile->y());
	ui.video->resize(this->size());
}

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
		ui.isPlay->setText(QString::fromLocal8Bit("�� ��"));
	}else {
		ui.isPlay->setText(QString::fromLocal8Bit("�� ͣ"));
	}
}
  
void XPlay2::openFile() {
	//ѡ���ļ�
	QString name = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("ѡ����Ƶ�ļ�"));
	//qDebug() << name;
	if (name.isEmpty()) return;
	this->setWindowTitle(name);

	if (!dt.Open(name.toLocal8Bit(), ui.video)) {
		QMessageBox::information(0, "error", "open file failed!");
		return;
	}

	SetPause(dt.isPause);
}

void XPlay2::PlayOrPause() {
	bool isPause = !dt.isPause;
	SetPause(isPause);
	dt.SetPause(isPause);
}

void XPlay2::SliderPress() {
	qDebug() << "SliderPress" << endl;
	isSliderPress = true;
}

void XPlay2::SliderRelease() {
	isSliderPress = false;
	double pos = 0.0;
	pos = (double)ui.playPos->value() / (double)ui.playPos->maximum();
	dt.Seek(pos);
}