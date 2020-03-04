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
	Mat img1 = imread("lena.jpg");
	Mat img2 = imread("test.jpg");
	resize(img2, img2, img1.size());
	Mat dst;
	float a = 0.5;
	addWeighted(img1, a, img2, 1 - a, 1, dst);
	namedWindow("blending");
	imshow("blending", dst);

	waitKey(0);

	return 0;
}