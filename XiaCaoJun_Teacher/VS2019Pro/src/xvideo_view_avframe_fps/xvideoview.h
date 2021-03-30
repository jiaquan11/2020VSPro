#pragma once
#ifndef XVIDEO_VIEW_H
#define XVIDEO_VIEW_H
////////////////////////
//��Ƶ��Ⱦ�ӿ���
//����SDLʵ��
//��Ⱦ���������
//�̰߳�ȫ
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
	//��ʼ����Ⱦ���� �̰߳�ȫ �ɶ�ε���
	//w:���ڿ��
	//h:���ڸ߶�
	//fmt:���Ƶ����ظ�ʽ
	//win_id:���ھ�������Ϊ��,�����´��� 
	//return �Ƿ񴴽��ɹ�
	virtual bool Init(int w, int h, Format fmt = RGBA, void* win_id=nullptr) = 0;

	//���������������Դ�������رմ���
	virtual void Close() = 0;

	//�������˳��¼�
	virtual bool IsExit() = 0;

	//��Ⱦͼ��  �̰߳�ȫ
	//data: ��Ⱦ�Ķ���������
	//linesize: һ�����ݵ��ֽ���,����YUV420P����Yһ�е��ֽ���
	//linesize<=0 �͸��ݿ�Ⱥ����ظ�ʽ�Զ������С
	//return ��Ⱦ�Ƿ�ɹ�
	virtual bool Draw(const unsigned char* data, int linesize = 0) = 0;
	virtual bool Draw(
		const unsigned char* y, int y_pitch,
		const unsigned char* u, int u_pitch,
		const unsigned char* v, int v_pitch) = 0;

	bool DrawFrame(AVFrame* frame);

	//��ʾ����
	void Scale(int w, int h) {
		scale_w_ = w;
		scale_h_ = h;
	}

public:
	int render_fps() {
		return render_fps_;
	}

protected:
	int render_fps_ = 0;//��ʾ֡��

	int width_ = 0;	//���ʿ��
	int height_ = 0;
	Format fmt_ = RGBA;	//���ظ�ʽ

	std::mutex mtx_;//ȷ���̰߳�ȫ

	int scale_w_ = 0;//��ʾ��С
	int scale_h_ = 0;
	long long beg_ms_ = 0; //��ʱ��ʼʱ��
	int count_ = 0;//ͳ����ʾ����
};
#endif

