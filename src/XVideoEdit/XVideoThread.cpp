#include "XVideoThread.h"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "XFilter.h"
#include <iostream>
using namespace cv;
using namespace std;

#pragma comment(lib, "opencv_world420d.lib")

//一号视频源
static VideoCapture cap1;
//二号视频源
static VideoCapture cap2;

//保存视频
static VideoWriter vw;

static bool isExit = false;

XVideoThread::XVideoThread(){
	start();
}

XVideoThread::~XVideoThread(){
	mutex.lock();
	isExit = true;
	mutex.unlock();
	wait();
}

//线程入口函数
void XVideoThread::run() {
	Mat mat1;
	for (;;){
		mutex.lock();
		if (isExit) {
			mutex.unlock();
			break;
		}

		//判断视频是否打开
		if (!cap1.isOpened()) {
			mutex.unlock();
			msleep(5);
			continue;
		}

		if (!isPlay) {
			mutex.unlock();
			msleep(5);
			continue;
		}

		int cur = cap1.get(CAP_PROP_POS_FRAMES);

		//读取一帧视频，解码并颜色转换
		if (((end > 0) && (cur >= end)) || !cap1.read(mat1) || mat1.empty()) {
			mutex.unlock();
			//导出到结尾位置，停止导出
			if (isWrite) {
				stopSave();
				SaveEnd();
			}
			msleep(5);
			continue;
		}

		Mat mat2 = mark;
		if (cap2.isOpened()) {
			cap2.read(mat2);
		}

		//显示图像 发送信号
		if (!isWrite) {//视频导出时不渲染
			ViewImage1(mat1);
			if (!mat2.empty()) {
				ViewImage2(mat2);
			}
		}

		//通过过滤器处理视频
		Mat des = XFilter::Get()->Filter(mat1, mat2);

		//显示生成后的图像 发送信号
		if (!isWrite)
			ViewDes(des);

		int s = 1000 / fps;

		if (isWrite) {
			s = 1;//导出视频时，降低sleep时间
			vw.write(des);
		}

		msleep(s);

		mutex.unlock();
	}
}

//打开一号视频源文件
bool XVideoThread::open(const std::string file) {
	cout << "open: " << file << endl;
	
	seek(0);

	mutex.lock();
	int ret = cap1.open(file);
	mutex.unlock();
	cout << ret << endl;

	if (!ret) {
		return ret;
	}

	fps = cap1.get(CAP_PROP_FPS);
	if (fps <= 0) fps = 25;
	width = cap1.get(CAP_PROP_FRAME_WIDTH);
	height = cap1.get(CAP_PROP_FRAME_HEIGHT);

	srcFile1 = file;

	double count = cap1.get(CAP_PROP_FRAME_COUNT);
	totalMs = (count / (double)fps) * 1000;

	return ret;
}

//打开二号视频源文件
bool XVideoThread::open2(const std::string file) {
	cout << "open2: " << file << endl;

	seek(0);

	mutex.lock();
	int ret = cap2.open(file);
	mutex.unlock();
	cout << ret << endl;

	if (!ret) {
		return ret;
	}

	//fps = cap1.get(CAP_PROP_FPS);
	//if (fps <= 0) fps = 25;
	width2 = cap2.get(CAP_PROP_FRAME_WIDTH);
	height2 = cap2.get(CAP_PROP_FRAME_HEIGHT);

	return ret;
}

//返回当前的播放位置
double XVideoThread::getPos() {
	double pos = 0;
	mutex.lock();
	if (!cap1.isOpened()) {
		mutex.unlock();
		return pos;
	}

	double cur = cap1.get(CAP_PROP_POS_FRAMES);
	double count = cap1.get(CAP_PROP_FRAME_COUNT);
	if (count > 0.001) {
		pos = cur / count;
	}

	mutex.unlock();
	return pos;
}

//跳转视频 frame:帧位置
bool XVideoThread::seek(int frame) {
	mutex.lock();
	if (!cap1.isOpened()) {
		mutex.unlock();
		return false;
	}
	bool ret = cap1.set(CAP_PROP_POS_FRAMES, frame);
	if (cap2.isOpened()) {
		cap2.set(CAP_PROP_POS_FRAMES, frame);
	}
	
	mutex.unlock();
	return ret;
}

bool XVideoThread::seek(double pos) {
	double count = cap1.get(CAP_PROP_FRAME_COUNT);
	int frame = pos * count;
	return seek(frame);
}

void XVideoThread::setBegin(double p) {
	mutex.lock();
	double count = cap1.get(CAP_PROP_FRAME_COUNT);
	int frame = p * count;
	begin = frame;
	mutex.unlock();
}

void XVideoThread::setEnd(double p) {
	mutex.lock();
	double count = cap1.get(CAP_PROP_FRAME_COUNT);
	int frame = p * count;
	end = frame;
	mutex.unlock();
}

//开始保存视频
bool XVideoThread::startSave(const std::string filename, int width, int height, bool isColor) {
	cout << "开始导出" << endl;

	//保证完整导出
	seek(begin);

	mutex.lock();
	if (!cap1.isOpened()) {
		mutex.unlock();
		return false;
	}

	if (width <= 0)
		width = cap1.get(CAP_PROP_FRAME_WIDTH);
	if (height <= 0) {
		height = cap1.get(CAP_PROP_FRAME_HEIGHT);
	}

	vw.open(filename,
		VideoWriter::fourcc('X', '2', '6', '4'),
		this->fps,
		Size(width, height), isColor);
	if (!vw.isOpened()) {
		mutex.unlock();
		cout << "start save failed!" << endl;
		return false;
	}

	this->isWrite = true;

	destFile = filename;

	mutex.unlock();
	return true;
}

//停止保存视频，写入视频帧的索引
void XVideoThread::stopSave() {
	cout << "停止导出" << endl;
	mutex.lock();
	vw.release();
	this->isWrite = false;
	mutex.unlock();
}

//添加视频水印
void XVideoThread::setMark(cv::Mat mark) {
	mutex.lock();
	this->mark = mark;
	mutex.unlock();
}