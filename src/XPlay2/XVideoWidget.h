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
	//不管成功与否，都释放frame空间
	virtual void Repaint(AVFrame* frame);

protected:
	//刷新显示
	void paintGL();
	//初始化
	void initializeGL();
	//窗口尺寸变化
	void resizeGL(int width, int height);

private:
	std::mutex mux;

	//shader程序
	QGLShaderProgram program;
	//shader中的yuv变量地址
	GLuint unis[3] = { 0 };
	//opengl的texture的地址
	GLuint texs[3] = { 0 };

	//材质内存空间
	unsigned char* datas[3] = { 0 };

	int width = 240;
	int height = 128;
};
