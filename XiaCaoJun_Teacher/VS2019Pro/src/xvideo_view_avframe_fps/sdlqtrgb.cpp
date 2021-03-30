#include "sdlqtrgb.h"
#include <sdl/SDL.h>
#include <fstream>
#include <iostream>
#include <QMessageBox>
#include <sstream>
#include <thread>
#include <QSpinBox>
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
static long long file_size = 0;
static QLabel* view_fps = nullptr;//��ʾfps�ؼ�
static QSpinBox* set_fps = nullptr;//����fps�ؼ�
int fps = 25;//����֡��

void SdlQtRGB::View() {
    yuv_file.read((char*)frame->data[0], sdl_width * sdl_height);//��ȡY
    yuv_file.read((char*)frame->data[1], sdl_width * sdl_height / 4);//��ȡU
    yuv_file.read((char*)frame->data[2], sdl_width * sdl_height / 4);//��ȡV
    if (yuv_file.tellg() == file_size) {//��ȡ���ļ���β
        yuv_file.seekg(0, ios::beg);
    }

    if (view->IsExit()) {
        view->Close();
        exit(0);
    }

    view->DrawFrame(frame);
    stringstream ss;
    ss << "fps: " << view->render_fps();

    //ֻ���ڲۺ����е���
    view_fps->setText(ss.str().c_str());
    fps = set_fps->value();//�õ�����֡��
}

void SdlQtRGB::Main() {
    while (!is_exit_) {
        ViewS();
        //this_thread::sleep_for(40ms);
        if (fps > 0) {
            MSleep(1000/fps);
        }
        else {
            MSleep(10);
        }
    }
}

void SdlQtRGB::timerEvent(QTimerEvent* ev) {

}

void SdlQtRGB::resizeEvent(QResizeEvent* ev) {
    ui.label->resize(size());
    ui.label->move(0, 0);

    //view->Scale(width(), height());
}

SdlQtRGB::SdlQtRGB(QWidget *parent)
    : QWidget(parent)
{
    //��yuv�ļ�
    yuv_file.open("400_300_25.yuv", ios::binary);
    if (!yuv_file) {
        QMessageBox::information(this, "", "open yuv file failed!");
        return;
    }
    yuv_file.seekg(0, ios::end);//�Ƶ��ļ���β
    file_size = yuv_file.tellg();//�ļ�ָ��λ��
    yuv_file.seekg(0, ios::beg);

    ui.setupUi(this);
    
    //����Ⱦ�źŲ�
    connect(this, SIGNAL(ViewS()), this, SLOT(View()));

    //��ʾfps�Ŀؼ�
    view_fps = new QLabel(this);
    view_fps->setText("fps: 100");

    //����fps
    set_fps = new QSpinBox(this);
    set_fps->move(200, 0);
    set_fps->setValue(25);
    set_fps->setRange(1, 300);

    sdl_width = 400;
    sdl_height = 300;

    ui.label->resize(sdl_width, sdl_height);

    view = XVideoView::Create();
    view->Init(sdl_width, sdl_height,
        XVideoView::YUV420P);

    view->Close();
    view->Init(sdl_width, sdl_height,
        XVideoView::YUV420P, (void*)ui.label->winId());

    //����frame����ռ�
    frame = av_frame_alloc();
    frame->width = sdl_width;
    frame->height = sdl_height;
    frame->format = AV_PIX_FMT_YUV420P;
    frame->linesize[0] = sdl_width;
    frame->linesize[1] = sdl_width/2;
    frame->linesize[2] = sdl_width/2;
    //����ͼ��ռ䣬Ĭ��32�ֽڶ���
    auto ret = av_frame_get_buffer(frame, 0);
    if (ret != 0) {
        char buf[1024] = { 0 };
        av_strerror(ret, buf, sizeof(buf));
        cerr << buf << endl;
    }

    //startTimer(10);
    th_ = std::thread(&SdlQtRGB::Main, this);
}