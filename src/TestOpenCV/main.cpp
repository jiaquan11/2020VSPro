#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
using namespace cv;

#pragma comment(lib, "opencv_world420d.lib")

int main(int argc, char* argv) {
	Mat image = imread("lena.jpg");
	namedWindow("img");
	imshow("img", image);

	waitKey(0);
	return 0;
}