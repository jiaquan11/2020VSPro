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

	//返回当前的播放位置
	double getPos();

	//跳转视频 frame:帧位置
	bool seek(int frame);
	bool seek(double pos);

	//开始保存视频
	bool startSave(const std::string filename, int width = 0, int height = 0);

	//停止保存视频，写入视频帧的索引
	void stopSave();
signals:
	//显示原视频1图像
	void ViewImage1(cv::Mat mat);

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

public:
	int fps = 0;
};

