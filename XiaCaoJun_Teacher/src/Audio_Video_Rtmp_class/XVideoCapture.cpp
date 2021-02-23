#include "XVideoCapture.h"
#include <opencv2/highgui.hpp>
#include <iostream>
using namespace cv;
using namespace std;
#pragma comment(lib, "opencv_world420d.lib")

class CXVideoCapture : public XVideoCapture {
public:
	VideoCapture cam;

public:
	void run() {
		Mat frame;
		while (!isExit) {
			if (!cam.read(frame)) {
				msleep(1);
				continue;
			}
			if (frame.empty()) {
				msleep(1);
				continue;
			}

			fmutex.lock();
			int num = filters.size();
			for (int i = 0; i < num; i++) {
				Mat des;
				filters[i]->Filter(&frame, &des);
				frame = des;
			}
			fmutex.unlock();

			//确保数据是连续的
			long long pts = GetCurTime();
			XData d((char*)frame.data, frame.cols*frame.rows*frame.elemSize(), pts);
			Push(d);
		}
	}

	bool Init(int camIndex = 0) {
		cam.open(camIndex);
		if (!cam.isOpened()) {
			cout << "cam open failed!" << endl;
			return false;
		}
		cout << camIndex << " cam open success!" << endl;
		width = cam.get(CAP_PROP_FRAME_WIDTH);
		height = cam.get(CAP_PROP_FRAME_HEIGHT);
		fps = cam.get(CAP_PROP_FPS);
		if (fps == 0) fps = 25;
		return true;
	}

	bool Init(const char *url) {
		cam.open(url);
		if (!cam.isOpened()) {
			cout << "cam open failed!" << endl;
			return false;
		}
		cout << url << " cam open success!" << endl;
		width = cam.get(CAP_PROP_FRAME_WIDTH);//摄像头宽
		height = cam.get(CAP_PROP_FRAME_HEIGHT);//摄像头高
		fps = cam.get(CAP_PROP_FPS);////摄像头帧率
		if (fps == 0) fps = 25;
		return true;
	}

	void Stop() {
		XDataThread::Stop();

		if (cam.isOpened()) {
			cam.release();
		}
	}
};

XVideoCapture::XVideoCapture() {

}

XVideoCapture::~XVideoCapture() {

}

XVideoCapture* XVideoCapture::Get(unsigned char index) {
	static CXVideoCapture cx[255];
	return &cx[index];
}
