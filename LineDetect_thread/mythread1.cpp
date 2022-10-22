#include "mythread1.h"

mythread1::mythread1(QObject *parent) : QObject(parent)
{
    //获取当前时间来进行录制视频文件的命名（以时间为文件名）
     time_t timep;//8字节的有符号整数，相当于长整型longint
     struct tm *p;
     char name[256] = {0};
     time(&timep);//获取从1970至今过了多少秒，存入time_t类型的timep
     p = localtime(&timep);//用localtime将秒数转化为struct tm结构体
     sprintf(name, "rec-%d-%d-%d-%d-%02d.avi",1900+p->tm_year,1+p->tm_mon,p->tm_mday,p->tm_hour,p->tm_min);

     //注意编码fourcc格式，和写入图像size要和摄像头读取的图像一致，不然可能写入失败 而且内存直接起飞
     writer.open(name,CV_FOURCC('M','J','P','G'), 30, cv::Size(640, 480),true);// true表示彩色帧
     if(!writer.isOpened())
     {
         std::cout<<"open file error\n";
         return;
     }

     isRec = true;


}

void mythread1::luzhi()
{
    if(frame.empty())
    {
        std::cout<<"图没有传进录制函数中，无法写入!"<<std::endl;
        return;
    }
    writer.write(frame);//write可以换成<<


           /****************
            if(!ui->Stop_rec->isEnabled())//press end rec
            {
                   writer.release();
                   std::cout<<"Video rec closed<<std::endl";
                   //std::cout.flush();
                   break;
            }
            ***************/

}
