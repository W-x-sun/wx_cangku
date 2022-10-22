#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define  _CRT_SECURE_NO_WARNINGS
#pragma execution_character_set("utf-8")//使得一些中文得以显示出来 不会乱码

#include <stdio.h>
#include <time.h>
#include <QTimer>
#include <QFileDialog>
#include <QMainWindow>
#include <QMessageBox>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
#include "mythread.h"
#include "mythread1.h"

using namespace cv;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:


    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();



private slots:

    void nextFrame();//
    void nextFrame_rec();//**********************************************1


    void on_OpenCamera_clicked();//

    void on_OpenVedio_clicked();//

    void on_CloseCamera_clicked();//

    void on_Stop_rec_clicked();//

    void on_Start_rec_clicked();//

    void dealclicked();//

    void stop_img_process_thread();//

    QImage Mat2QImage(cv::Mat cvImg);



private:
    Ui::MainWindow *ui;

        bool isRec;

        // CV 相关
        cv::Mat frame;
        cv::Mat mirror;
        cv::VideoCapture capture;
        QImage  image;
        QTimer *timer;

        QTimer * timer1;//***************************************************2

        double rate; //FPS
        cv::VideoWriter writer;   //make a video record

        //创建一个图像处理线程对象
        mythread *img_process_thread;//线程对象

        mythread1 *thread;//相当于创建了线程通道***********************************3
        QThread *luzhi_thread;//子线程*******************************************4
signals:
        //创建了一个信号***************************************************************5
    void rec_signal();

};

#endif // MAINWINDOW_H
