#ifndef MYTHREAD1_H
#define MYTHREAD1_H

#include <QObject>
#include <QThread>
#include <time.h>
#include <stdio.h>
#include <QTimer>
#include <QFileDialog>
#include <QMainWindow>
#include <QMessageBox>
#include <QMainWindow>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
using namespace cv;



class mythread1 : public QObject
{
    Q_OBJECT
public:
    explicit mythread1(QObject *parent = nullptr);
    cv::Mat frame;

public slots:
    void luzhi();//录制视频的处理函数
private:

    cv::VideoCapture capture;

    bool isRec;//和主线程定义了相同的变量，但不是同一个
    cv::VideoWriter writer;




signals:





public slots:
};

#endif // MYTHREAD1_H
