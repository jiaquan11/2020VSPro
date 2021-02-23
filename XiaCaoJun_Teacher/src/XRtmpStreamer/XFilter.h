#pragma once
#include <string>
#include <map>

enum XFilterType{
	XBILATERAL	//Ë«±ßÂË²¨
};

namespace cv {
	class Mat;
}

class XFilter
{
public:
	virtual ~XFilter();

	static XFilter *Get(XFilterType t = XBILATERAL);

	virtual bool Filter(cv::Mat *src, cv::Mat *des) = 0;
	virtual bool Set(std::string key, double value);

protected:
	XFilter();
	std::map<std::string, double> paras;
};

