#pragma once

#include <QtWidgets/QWidget>
#include "ui_sdlqtrgb.h"

class SdlQtRGB : public QWidget
{
    Q_OBJECT

public:
    SdlQtRGB(QWidget *parent = Q_NULLPTR);
    void timerEvent(QTimerEvent* ev) override;
    void resizeEvent(QResizeEvent* ev) override;

private:
    Ui::SdlQtRGBClass ui;
};
