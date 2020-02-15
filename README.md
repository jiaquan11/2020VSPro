# 2020VSPro
这里上传记录一些在2020年学习和编写的VS工程的相关工程代码，供今后查阅

1.TestDecode项目
实现整个ffmpeg的音视频解封装，数据包解析读取，音视频解码流程，seek操作，
音频重采样，视频尺寸变化及格式变化

2.TestDemux项目
实现整个ffmpeg音视频解封装，数据包解析读取

3.TestQAudioOutput项目
实现使用Qt音频接口播放pcm音频数据

4.TestQtOpengl项目
实现使用QT组件及opengl显示yuv文件播放

5.ViewFFmpeg项目
测试ffmepg在vs2015中的导入成功

6.VSTest项目
编写简单C语言代码测试VS2015是否安装成功

7.XPlay2项目
组装前四个项目的代码，通过多线程重构及QT界面，实现一个完整的Qt播放器框架。
可多次打开文件播放，可拖动进度条seek,可暂停并恢复播放，可全屏播放，可以尺寸变化播放，
音视频对齐。

8.XPlay项目
一个简单的QT和FFmpeg的视频播放项目，不完善，能播放而已

9.mp4_to_wmv项目
这是一个视频转封装的工程项目，将一个视频文件打开并循环读取数据包并写入一个新的
视频封装格式的文件中

10.rgb_to_mp4项目
增加一个读取rgb数据并转换为yuv进行编码，封装为mp4文件

11.pcm_to_aac项目
增加读取pcm数据，并进行编码封装为aac的工程

12.rgb_pcm_to_mp4项目
分别读取rgb文件和pcm文件，并进行编码封装合成mp4文件

13.TestDirectX项目
使用Directx接口进行屏幕截取生成rgb文件

14.