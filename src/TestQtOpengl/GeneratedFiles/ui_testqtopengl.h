/********************************************************************************
** Form generated from reading UI file 'testqtopengl.ui'
**
** Created by: Qt User Interface Compiler version 5.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TESTQTOPENGL_H
#define UI_TESTQTOPENGL_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QSlider>
#include <QtWidgets/QWidget>
#include "XVideoWidget.h"

QT_BEGIN_NAMESPACE

class Ui_TestQtOpenglClass
{
public:
    XVideoWidget *openGLWidget;
    QSlider *horizontalSlider;

    void setupUi(QWidget *TestQtOpenglClass)
    {
        if (TestQtOpenglClass->objectName().isEmpty())
            TestQtOpenglClass->setObjectName(QStringLiteral("TestQtOpenglClass"));
        TestQtOpenglClass->resize(1097, 798);
        openGLWidget = new XVideoWidget(TestQtOpenglClass);
        openGLWidget->setObjectName(QStringLiteral("openGLWidget"));
        openGLWidget->setGeometry(QRect(160, 50, 800, 600));
        horizontalSlider = new QSlider(TestQtOpenglClass);
        horizontalSlider->setObjectName(QStringLiteral("horizontalSlider"));
        horizontalSlider->setGeometry(QRect(190, 620, 751, 22));
        horizontalSlider->setOrientation(Qt::Horizontal);

        retranslateUi(TestQtOpenglClass);

        QMetaObject::connectSlotsByName(TestQtOpenglClass);
    } // setupUi

    void retranslateUi(QWidget *TestQtOpenglClass)
    {
        TestQtOpenglClass->setWindowTitle(QApplication::translate("TestQtOpenglClass", "TestQtOpengl", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class TestQtOpenglClass: public Ui_TestQtOpenglClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TESTQTOPENGL_H
