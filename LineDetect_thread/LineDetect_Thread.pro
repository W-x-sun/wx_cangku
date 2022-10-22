#-------------------------------------------------
#
# Project created by QtCreator 2022-06-16T21:55:52
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LineDetect_Thread

TEMPLATE = app #指定如何运行当前程序，默认值为 app，表示当前程序是一个应用程序，可以直接编译、运行。
               #常用的值还有 lib，表示将当前程序编译成库文件。

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0



SOURCES += \
        main.cpp \
        mainwindow.cpp \
    mythread.cpp \
    mythread1.cpp

HEADERS += \
        mainwindow.h \
    mythread.h \
    mythread1.h

FORMS += \
        mainwindow.ui
//1.添加include路径
INCLUDEPATH+="D:\caropencv\opencv\build\include\opencv2"
INCLUDEPATH+="D:\caropencv\opencv\build\include\opencv"
INCLUDEPATH+="D:\caropencv\opencv\build\include"
//2.配置静态库如下，两个版本debug和release版本3.若需要调用.dll文件只需要把这些文件放在该工程生成的相应文件夹debug或release下即可
CONFIG(debug,debug|release){
LIBS += -LD:/caropencv/opencv/build/x64/vc12/lib\
-lopencv_ml249d\
-lopencv_calib3d249d\
-lopencv_contrib249d\
-lopencv_core249d\
-lopencv_features2d249d\
-lopencv_flann249d\
-lopencv_gpu249d\
-lopencv_highgui249d\
-lopencv_imgproc249d\
-lopencv_legacy249d\
-lopencv_objdetect249d\
-lopencv_ts249d\
-lopencv_video249d\
-lopencv_nonfree249d\
-lopencv_ocl249d\
-lopencv_photo249d\
-lopencv_stitching249d\
-lopencv_superres249d\
-lopencv_videostab249d\
-lopencv_world341d
}
else{
LIBS += -LD:/caropencv/opencv/build/x64/vc12/lib\
-lopencv_objdetect249\
-lopencv_ts249\
-lopencv_video249\
-lopencv_nonfree249\
-lopencv_ocl249\
-lopencv_photo249\
-lopencv_stitching249\
-lopencv_superres249\
-lopencv_videostab249\
-lopencv_calib3d249\
-lopencv_contrib249\
-lopencv_core249\
-lopencv_features2d249\
-lopencv_flann249\
-lopencv_gpu249\
-lopencv_highgui249\
-lopencv_imgproc249\
-lopencv_legacy249\
-lopencv_ml249\
-lopencv_world341
}
