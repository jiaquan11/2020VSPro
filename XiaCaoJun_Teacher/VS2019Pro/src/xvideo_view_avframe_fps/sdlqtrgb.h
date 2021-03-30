#pragma once

#include <QtWidgets/QWidget>
#include "ui_sdlqtrgb.h"
#include <thread>

class SdlQtRGB : public QWidget
{
    Q_OBJECT

public:
    SdlQtRGB(QWidget *parent = Q_NULLPTR);
    ~SdlQtRGB() {
        is_exit_ = true;
        //�ȴ���Ⱦ�߳��˳�
        th_.join();
    }

    void timerEvent(QTimerEvent* ev) override;
    void resizeEvent(QResizeEvent* ev) override;
    //�̺߳���������ˢ����Ƶ
    void Main();

signals:
    void ViewS();//�źź���������������б�

public slots:
    void View();//��ʾ�Ĳۺ���

private:
    Ui::SdlQtRGBClass ui;
    std::thread th_;
    bool is_exit_ = false;//�����߳��˳�
};
