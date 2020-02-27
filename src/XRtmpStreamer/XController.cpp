#include "XController.h"
#include "XVideoCapture.h"
#include "XAudioRecord.h"
#include "XMediaEncode.h"
#include "XRtmp.h"
#include <iostream>
using namespace std;

XController::XController(){

}

XController::~XController(){

}

void XController::run() {
	long long beginTime = GetCurTime();//��׼��ʱ��ʼ��

	/*
	��Ƶ¼����������Ƶͬ���߼�:
	1.ͬʱ������Ƶ�ɼ��̺߳���Ƶ�ɼ��̣߳�����ȡ��ǰ�ɼ������ݵ�ʱ���
	2.����һ����׼��ʱ��ʼ��
	3.����Ƶ���Խ���ȡ���ݵ�ʱ����ȥʱ���׼��Ĳ�ֵ����Ϊ������ʱ���
	4.��˿��ԽϾ�׼�ر�֤����ͬ��
	*/
	while (!isExit) {
		//һ�ζ�ȡһ֡��Ƶ
		XData ad = XAudioRecord::Get()->Pop();
		XData vd = XVideoCapture::Get()->Pop();
		if ((ad.size <= 0) && (vd.size <= 0)) {
			msleep(1);
			continue;
		}

		//������Ƶ
		if (ad.size > 0) {
			ad.pts = ad.pts - beginTime;

			//�ز���Դ����
			XData pcm = XMediaEncode::Get()->Resample(ad);
			ad.Drop();

			XData pkt = XMediaEncode::Get()->EncodeAudio(pcm);
			if (pkt.size > 0) {
				//����
				if (XRtmp::Get()->SendFrame(pkt, aindex)) {
					cout << "#" << flush;
				}
			}
		}

		//������Ƶ
		if (vd.size > 0) {
			vd.pts = vd.pts - beginTime;
			//cout << "vd pts" <<vd.pts<< flush;
			XData yuv = XMediaEncode::Get()->RGBToYUV(vd);
			vd.Drop();

			XData pkt = XMediaEncode::Get()->EncodeVideo(yuv);
			cout << " V:"<<pkt.size << flush;
			if (pkt.size > 0) {
				//����
				if (XRtmp::Get()->SendFrame(pkt, vindex)) {
					cout << "@" << flush;
				}
			}
		}
	}
}

//�趨���ղ���
bool XController::Set(std::string key, double value) {
	XFilter::Get()->Set(key, value);
	return true;
}

bool XController::Start() {
	//1 ����ĥƤ������
	XVideoCapture::Get()->AddFilter(XFilter::Get());
	cout << "1 ����ĥƤ������" << endl;

	//2 �����
	if (camIndex >= 0) {
		if (!XVideoCapture::Get()->Init(camIndex)) {
			err = "2 ��ϵͳ���ʧ��";
			cout << err << endl;
			return false;
		}
	}
	else if (!inUrl.empty()) {
		if (!XVideoCapture::Get()->Init(inUrl.c_str())) {
			err = "2 ��";
			err += inUrl;
			err += "���ʧ��";
			cout << err << endl;
			return false;
		}
	}
	else {
		err = "2 �������������";
		cout << err << endl;
		return false;
	}
	cout << "2 ����򿪳ɹ�!" << endl;

	//3 QT��Ƶ��ʼ¼��
	if (!XAudioRecord::Get()->Init()) {
		err = "3 ¼���豸��ʧ��";
		cout << err << endl;
		return false;
	}
	cout << "3 ¼���豸�򿪳ɹ�!" << endl;

	//11 ��������Ƶ¼���߳�
	XAudioRecord::Get()->Start();
	XVideoCapture::Get()->Start();

	//����Ƶ������
	//4 ��ʼ����ʽת��������
	//��ʼ����������ݽṹ
	XMediaEncode::Get()->inWidth = XVideoCapture::Get()->width;
	XMediaEncode::Get()->inHeight = XVideoCapture::Get()->height;
	XMediaEncode::Get()->outWidth = XVideoCapture::Get()->width;
	XMediaEncode::Get()->outHeight = XVideoCapture::Get()->height;
	if (!XMediaEncode::Get()->InitScale()) {
		err = "4 ��Ƶ���ظ�ʽת����ʧ��";
		cout << err << endl;
		return false;
	}
	cout << "4 ��Ƶ���ظ�ʽת���򿪳ɹ�!" << endl;

	//5 ��Ƶ�ز��������ĳ�ʼ��
	XMediaEncode::Get()->channels = XAudioRecord::Get()->channels;
	XMediaEncode::Get()->nbSamples = XAudioRecord::Get()->nbSamples;
	XMediaEncode::Get()->sampleRate = XAudioRecord::Get()->sampleRate;
	if (!XMediaEncode::Get()->InitResample()) {
		err = "5 ��Ƶ�ز��������ĳ�ʼ��ʧ��!";
		cout << err << endl;
		return false;
	}
	cout << "5 ��Ƶ�ز��������ĳ�ʼ���ɹ�!" << endl;

	//6 ��ʼ����Ƶ������
	if (!XMediaEncode::Get()->InitAudioCodec()) {
		err = "6 ��ʼ����Ƶ������ʧ��!";
		cout << err << endl;
		return false;
	}

	//7 ��ʼ����Ƶ������
	if (!XMediaEncode::Get()->InitVideoCodec()) {
		err = "7 ��ʼ����Ƶ������ʧ��!";
		cout << err << endl;
		return false;
	}
	cout << "7 ��Ƶ��������ʼ���ɹ�!" << endl;

	//8 ���������װ��������
	if (!XRtmp::Get()->Init(outUrl.c_str())) {
		err = "8 ���������װ��������ʧ��!";
		cout << err << endl;
		return false;
	}
	cout << "8 ���������װ�������ĳɹ�" << endl;

	//9 �������Ƶ��
	vindex = XRtmp::Get()->AddStream(XMediaEncode::Get()->vc);
	aindex = XRtmp::Get()->AddStream(XMediaEncode::Get()->ac);
	if ((vindex < 0) || (aindex < 0)) {
		err = "9 �������Ƶ��ʧ��!";
		cout << err << endl;
		return false;
	}
	cout << "9 �������Ƶ���ɹ�!" << endl;

	//10 ���ͷ�װͷ
	if (!XRtmp::Get()->SendHead()) {
		err = "10 ���ͷ�װͷʧ��!";
		cout << err << endl;
		return false;
	}

	//����ǰ�����̵߳Ļ���������ȫ�����
	XAudioRecord::Get()->Clear();
	XVideoCapture::Get()->Clear();

	XDataThread::Start();

	return true;
}

void XController::Stop() {
	XDataThread::Stop();

	XAudioRecord::Get()->Stop();
	XVideoCapture::Get()->Stop();
	XMediaEncode::Get()->Close();
	XRtmp::Get()->Close();

	camIndex = -1;
	inUrl = "";
}