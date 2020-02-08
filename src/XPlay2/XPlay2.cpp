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
}

XPlay2::~XPlay2() {
	dt.Close();
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
