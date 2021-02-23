#include "XBilateralFilter.h"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
using namespace cv;
using namespace std;

XBilateralFilter::XBilateralFilter(){
	paras.insert(make_pair("b", 5));
}

XBilateralFilter::~XBilateralFilter(){

}

bool XBilateralFilter::Filter(cv::Mat *src, cv::Mat *des) {
	double d = paras["b"];
	bilateralFilter(*src, *des, d, d * 2, d / 2);
	return true;
}
