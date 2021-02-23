QT += core multimedia opengl widgets gui
CONFIG += debug console
DEFINES += QT_MULTIMEDIA_LIB QT_OPENGL_LIB QT_WIDGETS_LIB
win32{
INCLUDEPATH += ./GeneratedFiles \
    . \
    ./GeneratedFiles/Debug \
    ./../../include
LIBS += -L"./../../lib/win32" \
    -lopengl32 \
    -lglu32
DESTDIR = ../../bin/win32
}

linux{
DESTDIR = ../../bin/linux64
INCLUDEPATH += /usr/local/ffmpeg/include
LIBS += -L/usr/local/ffmpeg/lib -lavformat -lavutil -lavcodec -lswresample -lswscale
}

DEPENDPATH += .
OBJECTS_DIR += debug
include(XPlay.pri)

