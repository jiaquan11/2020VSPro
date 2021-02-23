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

void xresize(Mat& src, Mat& des, Size size) {
	des.create(size, src.type());
	//映射的原图坐标
	int sx, sy = 0;
	float fx = (float)src.cols / des.cols;
	float fy = (float)src.rows / des.rows;
	for (int x = 0; x < des.cols; x++){
		sx = fx * x;
		for (int y = 0; y < des.rows; y++){
			sy = fy * y;
			des.at<Vec3b>(y, x) = src.at<Vec3b>(sy, sx);
		}
	}
}

int main(int argc, char* argv) {
	Mat src = imread("lena.jpg");//512*512 256  1024
	Mat img256, img1024, des256, des1024;

	//xresize(src, img256, Size(256, 256));
	//xresize(src, img1024, Size(1024, 1024));

	//近邻插值算法缩放
	//resize(src, des256, Size(256, 256), 0, 0, INTER_NEAREST);
	//双线性插值
	resize(src, des1024, Size(1024, 1024), 0, 0, INTER_LINEAR);

	namedWindow("src");
	imshow("src", src);
	/*namedWindow("img256");
	imshow("img256", img256);
	namedWindow("img1024");
	imshow("img1024", img1024);*/

	//namedWindow("des256");
	//imshow("des256", des256);
	namedWindow("des1024");
	imshow("des1024", des1024);

	waitKey(0);

	return 0;
}