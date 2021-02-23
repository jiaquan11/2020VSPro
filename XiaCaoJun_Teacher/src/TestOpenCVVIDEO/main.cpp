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
	VideoCapture video;
	video.open("testvideo1.mp4");
	if (!video.isOpened()) {
		cout << "open video failed!" << endl;
		getchar();
		return -1;
	}
	cout << "open video success!" << endl;

	namedWindow("video");
	Mat frame;
	int fps = video.get(CAP_PROP_FPS);
	int s = 30;
	if (fps != 0) {
		s = 1000 / fps;
	}
	cout << "fps is " << fps << endl;
	int fcount = video.get(CAP_PROP_FRAME_COUNT);
	cout << "total frame is: " << fcount << endl;
	cout << "total sec is " << fcount / fps << endl;

	for (;;){
		/*if (!video.read(frame)) {
			break;
		}*/
		//读帧，解码
		if (!video.grab()) {
			break;
		}
		//转换颜色格式
		if (!video.retrieve(frame)) {
			break;
		}

		if (frame.empty()) break;

		imshow("video", frame);

		waitKey(s);
	}
	getchar();

	return 0;
}