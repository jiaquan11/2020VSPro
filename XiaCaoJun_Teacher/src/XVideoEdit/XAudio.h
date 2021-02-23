#pragma once
#include <string>

class XAudio
{
public:
	virtual ~XAudio();

	static XAudio* Get();

	//������Ƶ�ļ�
	//src:Դ��Ƶ�ļ�   out:�������Ƶ�ļ�
	//beginMs:��ʼ��Ƶλ�ú���
	//outMs:�����Ƶʱ��
	virtual bool ExportA(std::string src, std::string out, int beginMs=0, int outMs=0) = 0;

	//�ϲ�����Ƶ�ļ�
	virtual bool Merge(std::string v, std::string a, std::string out) = 0;

protected:
	XAudio();
};

