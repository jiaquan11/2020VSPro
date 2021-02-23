TEMPLATE = app
CONFIG += console c++11
SOURCES += main.cpp

win32{
INCLUDEPATH += $$PWD/../../include
#区分32位和64位windows程序
opt = $$find(QMAKESPEC,"msvc2015_64")
isEmpty(opt){
message("win32 lib")
LIBS += -L$$PWD/../../lib/win32
}
!isEmpty(opt){
message("win64 lib")
LIBS += -L$$PWD/../../lib/win64
}
}

#linux
linux{
INCLUDEPATH += /usr/local/ffmpeg/include
message("Linux")
LIBS += -L/usr/local/ffmpeg/lib -lavcodec -lavutil -lswresample
}
#message($$QMAKESPEC)
