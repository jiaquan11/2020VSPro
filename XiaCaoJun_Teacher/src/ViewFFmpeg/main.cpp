#include <iostream>
using namespace std;
//Ҫ����C���Ժ���
extern "C" {
#include<libavcodec/avcodec.h>
}

//Ԥ����ָ����
#pragma comment(lib, "avcodec.lib")
int main(int argc, char* argv[]) {
	//��ʾffmpeg�ı�������
	cout << "Test FFmpeg.club" << endl;

#ifdef _WIN32 //32λ��64λ win
    #ifdef _WIN64 //64 win
        cout << "Windows X64" << endl;
    #else
        cout << "Windows X86" << endl;
    #endif
#else
    cout << "Linux" << endl;
 #endif
	cout << avcodec_configuration() << endl;
	getchar();
	return 0;
}
