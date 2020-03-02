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

void RGBToGray(Mat &src, Mat &des) {
	//GRay = (R*30 + G*50 + B*11 + 50)/100
	des.create(src.rows, src.cols, CV_8UC1);
	for (int r = 0; r < src.rows; r++){
		for (int c = 0; c < src.cols; c++){
			Vec3b &m = src.at<Vec3b>(r, c);
			int gray = (m[2] * 30 + m[1] * 59 + m[0] * 11 + 50) / 100;
			des.at<uchar>(r, c) = gray;
		}
	}
}

int main(int argc, char* argv) {
	Mat src = imread("lena.jpg");
	Mat gray;
	PrintMs();
	cvtColor(src, gray, COLOR_BGR2GRAY);
	PrintMs("cvtColor");
	//////////////
	Mat mygray;
	PrintMs();
	RGBToGray(src, mygray);
	PrintMs("RGBToGray");

	namedWindow("mygray");
	imshow("mygray", mygray);
	///////////////
	namedWindow("gray");
	namedWindow("src");
	imshow("gray", gray);
	imshow("src", src);
	waitKey(0);

	return 0;
}