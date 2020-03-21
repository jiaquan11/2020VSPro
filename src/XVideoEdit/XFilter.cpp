#include "XFilter.h"
#include <QMutex>
#include "XImagePro.h"

class CXFilter : public XFilter {
public:
	std::vector<XTask> tasks;
	QMutex mutex;

public:
	virtual cv::Mat Filter(cv::Mat mat1, cv::Mat mat2){
		mutex.lock();
		XImagePro p;
		p.Set(mat1, mat2);

		for (int i = 0; i < tasks.size(); i++) {
			switch (tasks[i].type)
			{
			case XTASK_GAIN:
				//亮度和对比度调整
				p.Gain(tasks[i].para[0], tasks[i].para[1]);
				break;

			default:
				break;
			}
		}

		cv::Mat ret = p.Get();
		mutex.unlock();
		return ret;
	}

	virtual void Add(XTask task) {
		mutex.lock();
		tasks.push_back(task);
		mutex.unlock();
	}

	virtual void Clear() {
		mutex.lock();
		tasks.clear();
		mutex.unlock();
	}
};

XFilter::XFilter(){

}

XFilter::~XFilter(){

}

XFilter* XFilter::Get() {
	static CXFilter cx;
	return &cx;
}