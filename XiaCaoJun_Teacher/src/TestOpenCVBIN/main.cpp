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
	Mat bin;
	Mat gray;
	Mat ibin;

	cvtColor(src, gray, COLOR_BGR2GRAY);
	//二进制阈值化
	threshold(gray, bin, 100, 255, THRESH_BINARY);
	//反二进制阈值化
	threshold(gray, ibin, 100, 255, THRESH_BINARY_INV);

	namedWindow("src");
	namedWindow("bin");
	namedWindow("ibin");
	imshow("src", src);
	imshow("bin", bin);
	imshow("ibin", ibin);

	waitKey(0);

	return 0;
}