#pragma once
#include <QThread>
#include <QMutex>
#include <iostream>
using namespace std;

class XCaptureThread : protected QThread
{
public:
	//out
	int width = 1280;//��ȡ��Ļ�Ŀ�
	int height = 720;//��ȡ��Ļ�ĸ�

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
	//�̰߳�ȫ�����صĿռ����û��ͷ�
	char* GetRGB();

	virtual ~XCaptureThread();

protected:
	bool isExit = false;
	std::list<char*> rgbs;
	QMutex mutex;

	XCaptureThread();
};

