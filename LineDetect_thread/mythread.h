#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QThread>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
using namespace cv;
class mythread : public QThread
{
    Q_OBJECT
public:
    explicit mythread(QObject *parent = nullptr);

    bool picture_flag;//开始接收图像
   // Mat yuantu;int x2_last = 0;
    int width;
    int height;
    int count;




    cv::VideoCapture capture;
     cv::Mat frame;

    void on_ImageProcess_clicked(cv::Mat frame);
    void detect_line(Mat &yuantu);//Mat类负责在OpenCV中声明一个图像类型的变量。用来存放图像数据。
    Mat fitLines(Mat &img);

protected:

    void run();//多线程执行的内容将通过重新该虚函数实现

signals:
   // void over();


public slots:
};

#endif // MYTHREAD_H
