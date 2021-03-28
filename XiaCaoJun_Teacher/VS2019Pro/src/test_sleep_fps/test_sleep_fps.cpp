#include <iostream>
using namespace std;
#include <thread>
#include <windows.h>

void MSleep(unsigned int ms) {
    auto beg = clock();
    for (int i = 0; i < ms; i++) {
        this_thread::sleep_for(1ms);
        if ((clock() - beg) / (CLOCKS_PER_SEC / 1000) >= ms) {
            break;
        }
    }
}

int main()
{
    //测试C++11的sleep
    //测试sleep 10毫秒 100fps
    auto beg = clock();//开始时间 cpu跳数
    int fps = 0;//帧率
    for (;;) {
        fps++;
        auto tmp = clock();
        this_thread::sleep_for(10ms);
        //cout << clock() - tmp << " " << flush;

        //1秒钟开始统计， CLOCKS_PER_SEC
        if ((clock() - beg) / (CLOCKS_PER_SEC / 1000) >= 1000) //间隔毫秒数
        {
            cout << "sleep for fps: " << fps << endl;
            break;
        }
    }
    
    //测试wait 事件超时控制，帧率
    auto handle = CreateEvent(NULL, FALSE, FALSE, NULL);
    fps = 0;
    beg = clock();
    for (;;) {
        fps++;
        WaitForSingleObject(handle, 10);//等待超时

        //1秒钟开始统计， CLOCKS_PER_SEC
        if ((clock() - beg) / (CLOCKS_PER_SEC / 1000) >= 1000) //间隔毫秒数
        {
            cout << "wait fps: " << fps << endl;
            break;
        }
    }

    beg = clock();//开始时间 cpu跳数
    fps = 0;//帧率
    for (;;) {
        fps++;

        MSleep(10);

        //1秒钟开始统计， CLOCKS_PER_SEC
        if ((clock() - beg) / (CLOCKS_PER_SEC / 1000) >= 1000) {//间隔毫秒数
            cout << "MSleep fps: " << fps << endl;
            break;
        }
    }

    getchar();
    return 0;
}

