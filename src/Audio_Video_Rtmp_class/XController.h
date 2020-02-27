#pragma once
#include "XDataThread.h"

class XController : public XDataThread
{
public:
	std::string outUrl;
	int camIndex = -1;
	std::string inUrl = "";
	std::string err = "";

public:
	virtual ~XController();

	static XController *Get() {
		static XController xc;
		return &xc;
	}

	//�趨���ղ���
	virtual bool Set(std::string key, double value);
	virtual bool Start();
	virtual void Stop();
	void run();

protected:
	XController();

protected:
	int vindex = 0;//��Ƶ��������
	int aindex = 1;//��Ƶ��������
};

