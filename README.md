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