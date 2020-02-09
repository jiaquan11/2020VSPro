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
	long long total = dt.totalMs;
	if (total > 0) {
		double pos = (double)dt.pts / (double)total;
		int v = ui.playPos->maximum() * pos;
		ui.playPos->setValue(v);
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
}
