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
	Mat src = imread("lena.jpg");
	Mat gsrc;
	Mat lsrc;

	pyrDown(src, gsrc);//高斯金字塔  成倍向下采样
	pyrUp(src, lsrc);//拉普拉斯金字塔 向上采样

	namedWindow("src");
	moveWindow("src", 0, 0);
	namedWindow("gsrc");
	moveWindow("gsrc", 512, 0);
	namedWindow("lsrc");
	moveWindow("lsrc", 0, 512);

	imshow("src", src);
	imshow("gsrc", gsrc);
	imshow("lsrc", lsrc);

	waitKey(0);

	return 0;
}