#pragma once
#include "opencv2/core.hpp"
#include <vector>

enum XTaskType {
	XTASK_NONE,
	XTASK_GAIN,//亮度和对比度调整
};

struct XTask {
	XTaskType type;
	std::vector<double> para;
};

class XFilter
{
public:
	virtual ~XFilter();

	static XFilter* Get();

	virtual cv::Mat Filter(cv::Mat mat1, cv::Mat mat2) = 0;

	virtual void Add(XTask task) = 0;

	virtual void Clear() = 0;

protected:
	XFilter();
};

