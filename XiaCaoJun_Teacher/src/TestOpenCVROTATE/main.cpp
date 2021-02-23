#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
using namespace cv;
using namespace std;

#pragma comment(lib, "opencv_world420d.lib")

void PrintMs(const char* text = "") {
	static long long last = 0;
	long long cur = getTickCount();
	if (last == 0) {
		last = cur;
		return;
	}
	long long ms = 0;
	ms = ((double)(cur - last) / getTickFrequency()) * 1000;
	if (*text != 0) {
		printf("%s = %dms\n", text, ms);
	}
	last = getTickCount();
}

int main(int argc, char* argv) {
	Mat img = imread("lena.jpg");
	Mat rot;
	Mat fl;
	cv::rotate(img, rot, ROTATE_90_CLOCKWISE);
	cv::flip(img, fl, 0);//0表示以X轴镜像，即上下翻转

	namedWindow("rot");
	imshow("rot", rot);

	namedWindow("fl");
	imshow("fl", fl);

	waitKey(0);

	return 0;
}