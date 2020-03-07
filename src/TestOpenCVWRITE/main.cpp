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
	VideoCapture cam(0);
	if (!cam.isOpened()) {
		cout << "open cam failed!" << endl;
		getchar();
		return -1;
	}
	cout << "open cam success!" << endl;
	namedWindow("cam");
	Mat img;
	VideoWriter vw;
	int fps = cam.get(CAP_PROP_FPS);
	if (fps <= 0) {
		fps = 25;
	}
	vw.open("out.avi", VideoWriter::fourcc('X', '2', '6', '4'),
		fps,
		Size(cam.get(CAP_PROP_FRAME_WIDTH), 
			cam.get(CAP_PROP_FRAME_HEIGHT)));
	if (!vw.isOpened()) {
		cout << "VideoWriter open failed!" << endl;
		getchar();
		return -1;
	}
	cout << "VideoWriter open success!" << endl;

	for (;;){
		cam.read(img);
		if (img.empty()) break;
		imshow("cam", img);

		vw.write(img);

		if (waitKey(5) == 'q') break;
	}

	
	waitKey(0);

	return 0;
}