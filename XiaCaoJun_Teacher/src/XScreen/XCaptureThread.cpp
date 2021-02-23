#include "XCaptureThread.h"
#include <QTime>

#include <d3d9.h>
#pragma comment (lib, "d3d9.lib")

XCaptureThread::XCaptureThread(){

}

XCaptureThread::~XCaptureThread(){

}

//��ȡȫ��
void CaptureScreen(void *data) {
	//1.����directx3d����
	static IDirect3D9 *d3d = NULL;
	if (!d3d) {
		d3d = Direct3DCreate9(D3D_SDK_VERSION);
	}
	if (!d3d) return;

	//2.�����Կ����豸����
	static IDirect3DDevice9* device = NULL;
	if (!device) {
		D3DPRESENT_PARAMETERS pa;
		ZeroMemory(&pa, sizeof(pa));
		pa.Windowed = true;
		pa.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
		pa.SwapEffect = D3DSWAPEFFECT_DISCARD;
		pa.hDeviceWindow = GetDesktopWindow();

		d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, 0,
			D3DCREATE_HARDWARE_VERTEXPROCESSING, &pa, &device);
	}
	if (!device) return;

	//3 �����������滺��
	int w = GetSystemMetrics(SM_CXSCREEN);
	int h = GetSystemMetrics(SM_CYSCREEN);
	static IDirect3DSurface9* sur = NULL;
	if (!sur) {
		device->CreateOffscreenPlainSurface(w, h,
			D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH, &sur, 0);
	}
	if (!sur) return;
	if (!data) return;

	//4 ץ��
	device->GetFrontBufferData(0, sur);

	//5 ȡ������
	D3DLOCKED_RECT rect;
	ZeroMemory(&rect, sizeof(rect));
	if (sur->LockRect(&rect, 0, 0) != S_OK) {
		return;
	}
	memcpy(data, rect.pBits, w*h * 4);
	sur->UnlockRect();
};

void XCaptureThread::Start() {
	Stop();

	mutex.lock();
	isExit = false;
	CaptureScreen(0);//��NULL,����ǰ��ʼ������ʼ��
	width = GetSystemMetrics(SM_CXSCREEN);
	height = GetSystemMetrics(SM_CYSCREEN);
	mutex.unlock();

	start();
}

void XCaptureThread::Stop() {
	mutex.lock();
	isExit = true;
	while (!rgbs.empty()) {
		delete []rgbs.front();
		rgbs.pop_front();
	}
	mutex.unlock();

	wait();//�ȴ��߳��˳�
}

void XCaptureThread::run() {
	QTime t;

	//���н�������ͨ��֡�ʿ��ƽ���������
	while (!isExit) {
		t.restart();
		 
		mutex.lock();
		int s = 1000 / fps;
		if (rgbs.size() < cacheSize) {
			char* data = new char[width*height * 4];
			CaptureScreen(data);
			rgbs.push_back(data);
		}
		mutex.unlock();

		s = s - t.restart();
		if (s <= 0 || s > 10000) {
			s = 10;
		}
		msleep(s);
	}
}

char* XCaptureThread::GetRGB() {
	mutex.lock();
	if (rgbs.empty()) {
		mutex.unlock();
		return NULL;
	}
	char* ret = rgbs.front();
	rgbs.pop_front();
	mutex.unlock();
	
	return ret;
}
