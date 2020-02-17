#pragma once
#include <QMutex>
#include <QThread>

class XScreenRecord : protected QThread
{
public:
	//in  由外部界面值传入
	int fps = 10;
	int outWidth = 1280;
	int outHeight = 720;

	virtual ~XScreenRecord();

	static XScreenRecord* Get() {
		static XScreenRecord at;
		return &at;
	}

	bool Start(const char* filename);
	void Stop();
	void run();

protected:
	bool isExit = false;
	QMutex mutex;

protected:
	XScreenRecord();
};
