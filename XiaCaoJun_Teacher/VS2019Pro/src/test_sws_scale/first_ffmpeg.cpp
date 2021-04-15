#include<iostream>
#include<fstream>
using namespace std;

extern "C" {
#include<libswscale/swscale.h>
}

//Ԥ����ָ����
#pragma comment(lib, "swscale.lib")

#define YUV_FILE "400_300_25.yuv"
#define RGBA_FILE "800_600_25.rgba"

int main(int argc, char* argv[]) {
	cout << "111_test_sws_scale" << endl;
	//ffmpeg -i test.mp4 -s 400*300 400_300_25.yuv
	//400*300 YUVתRGBA 800*600 ���浽�ļ�
	int width = 400;
	int height = 300;
	int rgb_width = 800;
	int rgb_height = 600;

	int yuv_linesize[3] = { width, width / 2, width / 2 };
	//YUV420Pƽ��洢��yyyy uu vv
	unsigned char* yuv[3] = { 0 };
	yuv[0] = new unsigned char[width * height];//Y
	yuv[1] = new unsigned char[width * height/4];//U
	yuv[2] = new unsigned char[width * height/4];//V

	//RGBA����洢rgba rgba
	unsigned char* rgba = new unsigned char[rgb_width * rgb_height * 4];
	int rgba_linesize = rgb_width * 4;

	ifstream ifs;
	ifs.open(YUV_FILE, ios::binary);
	if (!ifs) {
		cerr << "open " << YUV_FILE << " failed!" << endl;
		return -1;
	}

	ofstream ofs;
	ofs.open(RGBA_FILE, ios::binary);
	if (!ofs) {
		cerr << "open " << RGBA_FILE << " failed!" << endl;
		return -1;
	}

	SwsContext* yuv2rgb = nullptr;
	for (;;) {
		//��ȡYUV֡
		ifs.read((char*)yuv[0], width * height);
		ifs.read((char*)yuv[1], width * height / 4);
		ifs.read((char*)yuv[2], width * height / 4);
		if (ifs.gcount() == 0)
			break;

		//YUVתRGBA
		//�����Ĵ����ͻ�ȡ
		yuv2rgb = sws_getCachedContext(
		yuv2rgb,//ת�������ģ�NULL�´�������NULL�ж������в�����һ��ֱ�ӷ��أ���һ��������ǰȻ���ٴ���
		width, height,//������
		AV_PIX_FMT_YUV420P,//�������ظ�ʽ
		rgb_width, rgb_height,//������
		AV_PIX_FMT_RGBA,//��������ظ�ʽ
		SWS_FAST_BILINEAR,//ѡ��֧�ֱ仯���㷨��˫���Բ�ֵ
		0, 0, 0//����������
		);
		if (!yuv2rgb) {
			cerr << "sws_getCachedContext failed!" << endl;
			return -1;
		}

		unsigned char* data[1];
		data[0] = rgba;
		int lines[1] = { rgba_linesize };

		int ret = sws_scale(yuv2rgb, 
			yuv, //��������
			yuv_linesize, //�����������ֽ���
			0, 
			height,//����߶�
			data,//�������
			lines
		);
		cout << ret << " " << flush;

		ofs.write((char*)rgba, rgb_width * rgb_height * 4);
	}
	ofs.close();
	ifs.close();

	//RGBA תYUV420P
	//һ��Ҫ�Զ����ƴ�
	ifs.open(RGBA_FILE, ios::binary);
	SwsContext *rgb2yuv = nullptr;
	for (;;) {
		//��ȡRGBA֡
		ifs.read((char*)rgba, rgb_width * rgb_height * 4);
		if (ifs.gcount() == 0)
			break;

		//RGBAתYUV
		//�����Ĵ����ͻ�ȡ
		rgb2yuv = sws_getCachedContext(
			rgb2yuv,//ת�������ģ�NULL�´�������NULL�ж������в�����һ��ֱ�ӷ��أ���һ��������ǰȻ���ٴ���
			rgb_width, rgb_height,//������
			AV_PIX_FMT_RGBA,//�������ظ�ʽ
			width, height,//������
			AV_PIX_FMT_YUV420P,//��������ظ�ʽ
			SWS_FAST_BILINEAR,//ѡ��֧�ֱ仯���㷨��˫���Բ�ֵ
			0, 0, 0//����������
		);
		if (!rgb2yuv) {
			cerr << "sws_getCachedContext failed!" << endl;
			return -1;
		}

		unsigned char* data[1];
		data[0] = rgba;
		int lines[1] = { rgba_linesize };
		int ret = sws_scale(rgb2yuv,
			data, //��������
			lines, //�����������ֽ���
			0,
			rgb_height,//����߶�
			yuv,//�������
			yuv_linesize
		);
		cout << "("<<ret << ")" << flush;
	}

	delete yuv[0];
	delete yuv[1];
	delete yuv[2];
	delete rgba;
	ifs.close();

	return 0;
}