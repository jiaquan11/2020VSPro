#pragma once
#include "opencv2/core.hpp"
#include <vector>

enum XTaskType {
	XTASK_NONE,
	XTASK_GAIN,//亮度和对比度调整
	XTASK_ROTATE90,
	XTASK_ROTATE180,
	XTASK_ROTATE270,
	XTASK_FLIPX,
	XTASK_FLIPY,
	XTASK_FLIPXY,
	XTASK_RESIZE,
	XTASK_PYDOWN,
	XTASK_PYUP,
	XTASK_CLIP,
	XTASK_GRAY,
	XTASK_MASK,
	XTASK_BLEND,
	XTASK_MERGE,
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

