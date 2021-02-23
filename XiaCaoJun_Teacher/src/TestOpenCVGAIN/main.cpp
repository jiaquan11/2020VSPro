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

/*
para: a float �Աȶ� 1.0~3.0
para: b int ���� 0~100
*/
void ChangeGain(Mat& src, Mat& des, float a, int b) {
	//g(r, c) = a * f(r, c) + b
	des.create(src.rows, src.cols, src.type());
	for (int r = 0; r < src.rows; r++){
		for (int c = 0; c < src.cols; c++){
			for (int i = 0; i < 3; i++){
				des.at<Vec3b>(r, c)[i] = saturate_cast<uchar>(a * src.at<Vec3b>(r, c)[i] + b);
			}
		}
	}
}

int main(int argc, char* argv) {
	//�����ԱȶȺ�����
	Mat src = imread("lena.jpg");
	Mat des;
	PrintMs("");
	ChangeGain(src, des, 2.0, 50);
	PrintMs("ChangeGain");

	PrintMs("");
	Mat des2;
	src.convertTo(des2, -1, 2.0, 50);//�ڶ�������-1��ʾ��ԭͼһ��
	PrintMs("convertTo");

	namedWindow("src");
	imshow("src", src);
	namedWindow("des");
	imshow("des", des);
	namedWindow("des2");
	imshow("des2", des2);

	waitKey(0);

	return 0;
}