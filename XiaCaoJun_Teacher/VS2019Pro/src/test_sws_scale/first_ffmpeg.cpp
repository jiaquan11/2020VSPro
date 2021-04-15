#include<iostream>
#include<fstream>
using namespace std;

extern "C" {
#include<libswscale/swscale.h>
}

//预处理指令导入库
#pragma comment(lib, "swscale.lib")

#define YUV_FILE "400_300_25.yuv"
#define RGBA_FILE "800_600_25.rgba"

int main(int argc, char* argv[]) {
	cout << "111_test_sws_scale" << endl;
	//ffmpeg -i test.mp4 -s 400*300 400_300_25.yuv
	//400*300 YUV转RGBA 800*600 并存到文件
	int width = 400;
	int height = 300;
	int rgb_width = 800;
	int rgb_height = 600;

	int yuv_linesize[3] = { width, width / 2, width / 2 };
	//YUV420P平面存储：yyyy uu vv
	unsigned char* yuv[3] = { 0 };
	yuv[0] = new unsigned char[width * height];//Y
	yuv[1] = new unsigned char[width * height/4];//U
	yuv[2] = new unsigned char[width * height/4];//V

	//RGBA交叉存储rgba rgba
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
		//读取YUV帧
		ifs.read((char*)yuv[0], width * height);
		ifs.read((char*)yuv[1], width * height / 4);
		ifs.read((char*)yuv[2], width * height / 4);
		if (ifs.gcount() == 0)
			break;

		//YUV转RGBA
		//上下文创建和获取
		yuv2rgb = sws_getCachedContext(
		yuv2rgb,//转换上下文，NULL新创建，非NULL判断与现有参数是一致直接返回，不一致先清理当前然后再创建
		width, height,//输入宽高
		AV_PIX_FMT_YUV420P,//输入像素格式
		rgb_width, rgb_height,//输出宽高
		AV_PIX_FMT_RGBA,//输出的像素格式
		SWS_FAST_BILINEAR,//选择支持变化的算法，双线性插值
		0, 0, 0//过滤器参数
		);
		if (!yuv2rgb) {
			cerr << "sws_getCachedContext failed!" << endl;
			return -1;
		}

		unsigned char* data[1];
		data[0] = rgba;
		int lines[1] = { rgba_linesize };

		int ret = sws_scale(yuv2rgb, 
			yuv, //输入数据
			yuv_linesize, //输入数据行字节数
			0, 
			height,//输入高度
			data,//输出数据
			lines
		);
		cout << ret << " " << flush;

		ofs.write((char*)rgba, rgb_width * rgb_height * 4);
	}
	ofs.close();
	ifs.close();

	//RGBA 转YUV420P
	//一定要以二进制打开
	ifs.open(RGBA_FILE, ios::binary);
	SwsContext *rgb2yuv = nullptr;
	for (;;) {
		//读取RGBA帧
		ifs.read((char*)rgba, rgb_width * rgb_height * 4);
		if (ifs.gcount() == 0)
			break;

		//RGBA转YUV
		//上下文创建和获取
		rgb2yuv = sws_getCachedContext(
			rgb2yuv,//转换上下文，NULL新创建，非NULL判断与现有参数是一致直接返回，不一致先清理当前然后再创建
			rgb_width, rgb_height,//输入宽高
			AV_PIX_FMT_RGBA,//输入像素格式
			width, height,//输出宽高
			AV_PIX_FMT_YUV420P,//输出的像素格式
			SWS_FAST_BILINEAR,//选择支持变化的算法，双线性插值
			0, 0, 0//过滤器参数
		);
		if (!rgb2yuv) {
			cerr << "sws_getCachedContext failed!" << endl;
			return -1;
		}

		unsigned char* data[1];
		data[0] = rgba;
		int lines[1] = { rgba_linesize };
		int ret = sws_scale(rgb2yuv,
			data, //输入数据
			lines, //输入数据行字节数
			0,
			rgb_height,//输入高度
			yuv,//输出数据
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