#include "sdlqtrgb.h"
#include <sdl/SDL.h>
#include <fstream>
#include <iostream>
#include <QMessageBox>
#include "xvideoview.h"
using namespace std;

extern "C" {
#include <libavcodec/avcodec.h>
}


static int sdl_width = 0;
static int sdl_height = 0;

static int pix_size = 2;

static ifstream yuv_file;
static XVideoView* view = nullptr;
static AVFrame* frame = nullptr;

void SdlQtRGB::timerEvent(QTimerEvent* ev) {
    //yuv_file.read((char*)yuv, sdl_width * sdl_height * 1.5);
    
    yuv_file.read((char*)frame->data[0], sdl_width * sdl_height);//读取Y
    yuv_file.read((char*)frame->data[1], sdl_width * sdl_height/4);//读取U
    yuv_file.read((char*)frame->data[2], sdl_width * sdl_height/4);//读取V

    if (view->IsExit()) {
        view->Close();
        exit(0);
    }

    view->DrawFrame(frame);

    //view->Draw(yuv);
}

void SdlQtRGB::resizeEvent(QResizeEvent* ev) {
    ui.label->resize(size());
    ui.label->move(0, 0);

    //view->Scale(width(), height());
}

SdlQtRGB::SdlQtRGB(QWidget *parent)
    : QWidget(parent)
{
    //打开yuv文件
    yuv_file.open("400_300_25.yuv", ios::binary);
    if (!yuv_file) {
        QMessageBox::information(this, "", "open yuv file failed!");
        return;
    }

    ui.setupUi(this);
    
    sdl_width = 400;
    sdl_height = 300;

    ui.label->resize(sdl_width, sdl_height);

    view = XVideoView::Create();
    view->Init(sdl_width, sdl_height,
        XVideoView::YUV420P);

    view->Close();
    view->Init(sdl_width, sdl_height,
        XVideoView::YUV420P, (void*)ui.label->winId());

    //生成frame对象空间
    frame = av_frame_alloc();
    frame->width = sdl_width;
    frame->height = sdl_height;
    frame->format = AV_PIX_FMT_YUV420P;
    frame->linesize[0] = sdl_width;
    frame->linesize[1] = sdl_width/2;
    frame->linesize[2] = sdl_width/2;
    //生成图像空间，默认32字节对齐
    auto ret = av_frame_get_buffer(frame, 0);
    if (ret != 0) {
        char buf[1024] = { 0 };
        av_strerror(ret, buf, sizeof(buf));
        cerr << buf << endl;
    }

    startTimer(10);
}