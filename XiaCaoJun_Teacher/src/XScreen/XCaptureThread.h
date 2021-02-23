#pragma once
#include <QThread>
#include <QMutex>
#include <iostream>
using namespace std;

class XCaptureThread : protected QThread
{
public:
	//out
	int width = 1280;//截取屏幕的宽
	int height = 720;//截取屏幕的高

	//in
	int fps = 10;
	int cacheSize = 3;

	static XCaptureThread* Get() {
		static XCaptureThread ct;
		return &ct;
	}

	void Start();
	void Stop();
	void run();
	//线程安全，返回的空间由用户释放
	char* GetRGB();

	virtual ~XCaptureThread();

protected:
	bool isExit = false;
	std::list<char*> rgbs;
	QMutex mutex;

	XCaptureThread();
};

