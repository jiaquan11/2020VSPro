#pragma once
#include <string>

class XAudio
{
public:
	virtual ~XAudio();

	static XAudio* Get();

	//导出音频文件
	//src:源视频文件   out:输出的音频文件
	//beginMs:开始音频位置毫秒
	//outMs:输出音频时长
	virtual bool ExportA(std::string src, std::string out, int beginMs=0, int outMs=0) = 0;

	//合并音视频文件
	virtual bool Merge(std::string v, std::string a, std::string out) = 0;

protected:
	XAudio();
};

