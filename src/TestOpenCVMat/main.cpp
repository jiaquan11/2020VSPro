#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
using namespace cv;

#pragma comment(lib, "opencv_world420d.lib")

int main(int argc, char* argv) {
	Mat mat(3000, 4000, CV_8UC3);
	//mat.create(3000, 4000, CV_8UC3);
	int es = mat.elemSize();
	int size = mat.rows*mat.cols*es;

	for (int i = 0; i < size; i += es) {
		mat.data[i] = 255;//B
		mat.data[i + 1] = 0;//G
		mat.data[i + 2] = 0;//R
	}

	namedWindow("mat");
	imshow("mat", mat);
	waitKey(0);

	return 0;
}