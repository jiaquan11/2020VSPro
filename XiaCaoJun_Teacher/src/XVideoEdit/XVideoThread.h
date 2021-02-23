#pragma once
#include <QThread>
#include <QMutex>
#include <opencv2/core.hpp>

class XVideoThread : public QThread
{
	Q_OBJECT

public:
	~XVideoThread();

	//单例模式，获取对象
	static XVideoThread* Get() {
		static XVideoThread vt;
		return &vt;
	}

	//线程入口函数
	void run();

	//打开一号视频源文件
	bool open(const std::string file);
	//打开二号视频源文件
	bool open2(const std::string file);

	void play() {
		mutex.lock();
		isPlay = true;
		mutex.unlock();
	};

	void pause() { 
		mutex.lock();
		isPlay = false; 
		mutex.unlock();
	};

	//返回当前的播放位置
	double getPos();

	//跳转视频 frame:帧位置
	bool seek(int frame);
	bool seek(double pos);

	//开始保存视频
	bool startSave(const std::string filename, int width = 0, int height = 0, bool isColor = true);

	//停止保存视频，写入视频帧的索引
	void stopSave();

	//添加视频水印
	void setMark(cv::Mat mark);

signals:
	//显示原视频1图像
	void ViewImage1(cv::Mat mat);
	//显示原视频2图像
	void ViewImage2(cv::Mat mat);

	//显示输出视频图像
	void ViewDes(cv::Mat mat);

	//导出结束
	void SaveEnd();

protected:
	XVideoThread();

protected:
	QMutex mutex;
	//是否开始写视频
	bool isWrite = false;
	bool isPlay = false;
	cv::Mat mark;

public:
	int fps = 0;
	int width = 0;
	int height = 0;
	int width2 = 0;
	int height2 = 0;
	int totalMs = 0;

	std::string srcFile1;
	std::string destFile;

	int begin = 0;
	int end = 0;

	void setBegin(double p);

	void setEnd(double p);
};

