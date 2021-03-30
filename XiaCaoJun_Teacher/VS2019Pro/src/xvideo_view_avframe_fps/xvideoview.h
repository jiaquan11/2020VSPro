#pragma once
#ifndef XVIDEO_VIEW_H
#define XVIDEO_VIEW_H
////////////////////////
//视频渲染接口类
//隐藏SDL实现
//渲染方案可替代
//线程安全
#include <mutex>

struct AVFrame;

void MSleep(unsigned int ms);

class XVideoView{
public:
	enum Format {
		RGBA = 0,
		ARGB,
		YUV420P
	};
	enum RenderType {
		SDL = 0
	};
	static XVideoView* Create(RenderType type = SDL);

	////////
	//初始化渲染窗口 线程安全 可多次调用
	//w:窗口宽度
	//h:窗口高度
	//fmt:绘制的像素格式
	//win_id:窗口句柄，如果为空,创建新窗口 
	//return 是否创建成功
	virtual bool Init(int w, int h, Format fmt = RGBA, void* win_id=nullptr) = 0;

	//清理所有申请的资源，包括关闭窗口
	virtual void Close() = 0;

	//处理窗口退出事件
	virtual bool IsExit() = 0;

	//渲染图像  线程安全
	//data: 渲染的二进制数据
	//linesize: 一行数据的字节数,对于YUV420P就是Y一行的字节数
	//linesize<=0 就根据宽度和像素格式自动算出大小
	//return 渲染是否成功
	virtual bool Draw(const unsigned char* data, int linesize = 0) = 0;
	virtual bool Draw(
		const unsigned char* y, int y_pitch,
		const unsigned char* u, int u_pitch,
		const unsigned char* v, int v_pitch) = 0;

	bool DrawFrame(AVFrame* frame);

	//显示缩放
	void Scale(int w, int h) {
		scale_w_ = w;
		scale_h_ = h;
	}

public:
	int render_fps() {
		return render_fps_;
	}

protected:
	int render_fps_ = 0;//显示帧率

	int width_ = 0;	//材质宽高
	int height_ = 0;
	Format fmt_ = RGBA;	//像素格式

	std::mutex mtx_;//确保线程安全

	int scale_w_ = 0;//显示大小
	int scale_h_ = 0;
	long long beg_ms_ = 0; //计时开始时间
	int count_ = 0;//统计显示次数
};
#endif

