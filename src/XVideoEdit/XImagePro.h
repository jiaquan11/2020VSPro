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

	//图像旋转
	void Rotate90();
	void Rotate180();
	void Rotate270();

	//图像镜像
	void FlipX();
	void FlipY();
	void FlipXY();

	//图像尺寸
	void Resize(int width, int height);

	//金字塔
	void PyDown(int count);
	void PyUp(int count);

	//视频画面裁剪
	void Clip(int x, int y, int w, int h);

	//转为灰度图
	void Gray();

	//水印
	void Mark(int x, int y, double a);

	//视频融合
	void Blend(double a);

	//视频合并
	void Merge();

private:
	//原图
	cv::Mat src1, src2;
	//目标图
	cv::Mat des;
};

