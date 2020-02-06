#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QGLShaderProgram>
#include <mutex>
struct AVFrame;

class XVideoWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT

public:
	XVideoWidget(QWidget *parent);
	~XVideoWidget();

	void Init(int width, int height);
	//���ܳɹ���񣬶��ͷ�frame�ռ�
	virtual void Repaint(AVFrame* frame);

protected:
	//ˢ����ʾ
	void paintGL();
	//��ʼ��
	void initializeGL();
	//���ڳߴ�仯
	void resizeGL(int width, int height);

private:
	std::mutex mux;

	//shader����
	QGLShaderProgram program;
	//shader�е�yuv������ַ
	GLuint unis[3] = { 0 };
	//opengl��texture�ĵ�ַ
	GLuint texs[3] = { 0 };

	//�����ڴ�ռ�
	unsigned char* datas[3] = { 0 };

	int width = 240;
	int height = 128;
};
