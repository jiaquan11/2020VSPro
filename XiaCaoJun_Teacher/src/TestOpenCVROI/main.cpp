#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
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
	Rect rect(100, 100, 200, 200);
	Mat roi = src(rect);
	MatSize s = roi.size;

	namedWindow("roi", WINDOW_AUTOSIZE);
	namedWindow("src");
	imshow("roi", roi);
	imshow("src", src);
	waitKey(0);

	return 0;
}