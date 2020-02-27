#pragma once
#include "XFilter.h"
class XBilateralFilter : public XFilter
{
public:
	XBilateralFilter();
	virtual ~XBilateralFilter();

	bool Filter(cv::Mat *src, cv::Mat *des);
};

