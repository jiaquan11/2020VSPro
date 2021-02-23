#include "XAudio.h"
#include <QMutex>
#include <iostream>
using namespace std;

struct XTime {
	XTime(int tms) {
		h = (tms / 1000) / 3600;
		m = ((tms / 1000) % 3600) / 60;
		s = (tms / 1000) % (3600 * 60);
		ms = tms%1000;
 }

	std::string ToString() {
		char buf[16] = { 0 };
		sprintf(buf, "%d:%d:%d.%d", h, m, s, ms);
		return buf;
	}

	int h = 0;
	int m = 0;
	int s = 0;
	int ms = 0;
};

class CXAudio : public XAudio {
public:
	QMutex mutex;

public:
	//导出音频文件
	//src:源视频文件   out:输出的音频文件
	virtual bool ExportA(std::string src, std::string out, int beginMs, int outMs) {
		//ffmpeg -i test.mp4 -ss 0:1:10.112 -t 0:1:10.100 -vn -y test.mp3
		string cmd = "ffmpeg ";
		cmd += " -i ";
		cmd += src;
		cmd += " ";
		if (beginMs > 0) {
			cmd += " -ss ";
			XTime xt(beginMs);
			cmd += xt.ToString();
		}
		if (outMs > 0) {
			cmd += " -t ";
			XTime xt(outMs);
			cmd += xt.ToString();
		}

		cmd += " -vn -y ";
		cmd += out;
		cout << cmd << endl;
		mutex.lock();
		system(cmd.c_str());
		mutex.unlock();
		return true;
	}

	//合并音视频文件
	virtual bool Merge(std::string v, std::string a, std::string out) {
		//ffmpeg -i test.avi -i test.mp3 -c copy out.avi
		string cmd = "ffmpeg -i ";
		cmd += v;
		cmd += " -i ";
		cmd += a;
		cmd += " -c copy ";
		cmd += out;
		cout << cmd << endl;
		mutex.lock();
		system(cmd.c_str());
		mutex.unlock();
		return true;
	}
};

XAudio* XAudio::Get() {
	static CXAudio ca;
	return &ca;
}

XAudio::XAudio(){

}

XAudio::~XAudio(){

}
