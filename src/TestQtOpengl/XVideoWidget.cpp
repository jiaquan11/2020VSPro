#include "XVideoWidget.h"
#include <QDebug>
#include <QTimer>

//�Զ���˫����
#define GET_STR(x) #x
#define A_VER 3
#define T_VER 4

FILE *fp = NULL;

//����shader
const char* vString = GET_STR(
	attribute vec4 vertexIn;
	attribute vec2 textureIn;
	varying vec2 textureOut;
	void main(void) {
		gl_Position = vertexIn;
		textureOut = textureIn;
	}
);

//ƬԪshader
const char* tString = GET_STR(
	varying vec2 textureOut;
	uniform sampler2D tex_y;
	uniform sampler2D tex_u;
	uniform sampler2D tex_v;
	void main(void) {
		vec3 yuv;
		vec3 rgb;
		yuv.x = texture2D(tex_y, textureOut).r;
		yuv.y = texture2D(tex_u, textureOut).r - 0.5;
		yuv.z = texture2D(tex_v, textureOut).r - 0.5;
		rgb = mat3(1.0,1.0,1.0,
			0.0, -0.39465, 2.03211,
			1.13983, -0.58060, 0) * yuv;
		gl_FragColor = vec4(rgb, 1.0);
	}
);


//׼��yuv����
//ffmpeg -i testvideo.mp4 -t 10 -s 240*128 -pix_fmt yuv420p out240*128.yuv
XVideoWidget::XVideoWidget(QWidget *parent)
	: QOpenGLWidget(parent)
{
}

XVideoWidget::~XVideoWidget()
{
}

//��ʼ��GL
void XVideoWidget::initializeGL(){
	qDebug() << "initializeGL";
	//��ʼ��opengl (QOpenGLFunctions�̳�)����
	initializeOpenGLFunctions();

	//program����shader(�����ƬԪ)�ű�
	//����shader
	qDebug()<<program.addShaderFromSourceCode(QGLShader::Vertex, vString);
	//ƬԪ(����)
	qDebug() << program.addShaderFromSourceCode(QGLShader::Fragment, tString);

	//���ö�������ı���
	program.bindAttributeLocation("vertexIn", A_VER);
	//���ò�������
	program.bindAttributeLocation("textureIn", T_VER);

	//����shader
	qDebug() << "program link()" << program.link();
	qDebug() << "program bind()" << program.bind();

	//���ݶ���Ͳ�������
	//����
	static const GLfloat ver[] = {
		-1.0f, -1.0f,
		1.0f, -1.0f,
		-1.0f, 1.0f,
		1.0f, 1.0f
	};

	//����
	static const GLfloat tex[] = {
		0.0f, 1.0f,
		1.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f
	};

	//����
	glVertexAttribPointer(A_VER, 2, GL_FLOAT, 0, 0, ver);
	glEnableVertexAttribArray(A_VER);

	//����
	glVertexAttribPointer(T_VER, 2, GL_FLOAT, 0, 0, tex);
	glEnableVertexAttribArray(T_VER);

	//��shader��ȡ����
	unis[0] = program.uniformLocation("tex_y");
	unis[1] = program.uniformLocation("tex_u");
	unis[2] = program.uniformLocation("tex_v");

	//��������
	glGenTextures(3, texs);
	//Y
	glBindTexture(GL_TEXTURE_2D, texs[0]);
	//�Ŵ���ˣ����Բ�ֵ   GL_NEAREST(Ч�ʸߣ�������������)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//���������Կ��ռ�
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
	//U
	glBindTexture(GL_TEXTURE_2D, texs[1]);
	//�Ŵ���ˣ����Բ�ֵ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//���������Կ��ռ�
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width/2, height/2, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
	//V
	glBindTexture(GL_TEXTURE_2D, texs[2]);
	//�Ŵ���ˣ����Բ�ֵ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//���������Կ��ռ�
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width/2, height/2, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

	//��������ڴ�ռ�
	datas[0] = new unsigned char[width*height];//Y
	datas[1] = new unsigned char[width*height/4];//U
	datas[2] = new unsigned char[width*height/4];//V

	fp = fopen("out.yuv", "rb");
	if (!fp) {
		qDebug() << "out.yuv file open failed!";
	}

	//������ʱ��
	QTimer *ti = new QTimer(this);
	connect(ti, SIGNAL(timeout()), this, SLOT(update()));
	ti->start(33);
}

//ˢ����ʾ
void XVideoWidget::paintGL(){
	if (feof(fp)) {
		fseek(fp, 0, SEEK_SET);
	}
	fread(datas[0], 1, width*height, fp);
	fread(datas[1], 1, width*height/4, fp);
	fread(datas[2], 1, width*height/4, fp);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texs[0]);//0��󶨵�Y����
	//�޸Ĳ�������(�����ڴ�����)
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RED, GL_UNSIGNED_BYTE, datas[0]);
	//��shader uni��������
	glUniform1i(unis[0], 0);

	glActiveTexture(GL_TEXTURE0+1);
	glBindTexture(GL_TEXTURE_2D, texs[1]);//1��󶨵�U����
										  //�޸Ĳ�������(�����ڴ�����)
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width/2, height/2, GL_RED, GL_UNSIGNED_BYTE, datas[1]);
	//��shader uni��������
	glUniform1i(unis[1], 1);

	glActiveTexture(GL_TEXTURE0+2);
	glBindTexture(GL_TEXTURE_2D, texs[2]);//0��󶨵�V����
										  //�޸Ĳ�������(�����ڴ�����)
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width/2, height/2, GL_RED, GL_UNSIGNED_BYTE, datas[2]);
	//��shader uni��������
	glUniform1i(unis[2], 2);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	qDebug() << "paintGL";
}

//���ڳߴ�仯
void XVideoWidget::resizeGL(int width, int height){
	qDebug() << "resizeGL"<<width<<":"<<height;
}
