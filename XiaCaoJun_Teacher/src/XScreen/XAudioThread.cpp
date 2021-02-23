#include "XAudioThread.h"
#include <iostream>
#include <QAudioInput>
using namespace std;

static QAudioInput* input = NULL;
static QIODevice* io = NULL;

XAudioThread::XAudioThread(){

}

XAudioThread::~XAudioThread(){

}

void XAudioThread::Start() {
	Stop();

	mutex.lock();

	isExit = false;
	
	QAudioFormat fmt;
	fmt.setSampleRate(sampleRate);
	fmt.setChannelCount(channels);
	fmt.setSampleSize(sampleByte*8);
	fmt.setSampleType(QAudioFormat::UnSignedInt);
	fmt.setByteOrder(QAudioFormat::LittleEndian);
	fmt.setCodec("audio/pcm");
	input = new QAudioInput(fmt);
	io = input->start();

	mutex.unlock();

	start();//开启线程
}

void XAudioThread::Stop() {
	mutex.lock();
	isExit = true;
	while (!pcms.empty()) {
		delete []pcms.front();
		pcms.pop_front();
	}
	if (input) {
		io->close();
		input->stop();
		delete input;
		input = NULL;
		io = NULL;
	}
	mutex.unlock();

	wait();//等待线程退出
}

void XAudioThread::run() {
	int size = nbSample*channels*sampleByte;
	while (!isExit) {
		mutex.lock();
		if (pcms.size() > cacheSize) {
			mutex.unlock();
			msleep(5);
			continue;
		}
		char* data = new char[size];
		int readSize = 0;
		while (readSize < size) {
			int br = input->bytesReady();
			if (br < 1024) {
				msleep(1);
				continue;
			}
			int s = 1024;
			//最后一次
			if (size - readSize < s) {
				s = size - readSize;
			}
			int len = io->read(data + readSize, s);
			readSize += len;
		}
		pcms.push_back(data);

		mutex.unlock();
	}
}

char* XAudioThread::GetPCM() {
	mutex.lock();
	if (pcms.empty()) {
		mutex.unlock();
		return NULL;
	}
	char* ret = pcms.front();
	pcms.pop_front();
	mutex.unlock();
	
	return ret;
}