#include "TestQtMat.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	TestQtMat w;
	w.show();
	return a.exec();
}
