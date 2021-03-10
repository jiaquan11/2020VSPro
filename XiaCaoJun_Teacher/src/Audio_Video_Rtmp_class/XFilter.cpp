#include "XFilter.h"
#include "XBilateralFilter.h"
#include <iostream>
using namespace std;

XFilter::XFilter() {

}

XFilter::~XFilter() {

}

XFilter* XFilter::Get(XFilterType t) {
	static XBilateralFilter xbf;

	switch (t)
	{
	case XBILATERAL://Ë«±ßÂË²¨
		return &xbf;

	default:
		break;
	}
}

bool XFilter::Set(std::string key, double value) {
	if (paras.find(key) == paras.end()) {
		cout << "para " << key << " is not support!" << endl;
		return false;
	}

	paras[key] = value;
	return true;
}