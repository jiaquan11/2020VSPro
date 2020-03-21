#include "XImagePro.h"

XImagePro::XImagePro(){

}


XImagePro::~XImagePro(){

}

//设置原图，会清理处理结果
void XImagePro::Set(cv::Mat mat1, cv::Mat mat2) {
	this->src1 = mat1;
	this->src2 = mat2;
	this->src1.copyTo(des);
}

//设置对比度和亮度
//对比度:1.0~3.0 亮度:0~100, 
void XImagePro::Gain(double contrast, double bright) {
	if (des.empty()) return;
	des.convertTo(des, -1, contrast, bright);
}
