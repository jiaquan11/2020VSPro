#pragma once
#include "opencv2/core.hpp"

class XImagePro
{
public:
	XImagePro();
	~XImagePro();

	//设置原图，会清理处理结果
	void Set(cv::Mat mat1, cv::Mat mat2);

	//获取处理后的结果
	cv::Mat Get() {
		return des;
	}
	
	//设置对比度和亮度
	//对比度:1.0~3.0 亮度:0~100, 
	void Gain(double contrast, double bright);

private:
	//原图
	cv::Mat src1, src2;
	//目标图
	cv::Mat des;
};

