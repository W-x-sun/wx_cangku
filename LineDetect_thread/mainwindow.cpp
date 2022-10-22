#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mythread.h"
#include "mythread1.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
   //this->setWindowTitle(QStringLiteral("车道线检测"));设置窗口名称，等价于main.cpp里的w.setwindowtitle语句

    //ui->display->setMargin(30);设置表示控件与窗体的左右边距
    ui->display->setScaledContents(true);//将视频安装到标签区域，scaled为按比例缩放
    ui->Stop_rec->setDisabled(true);
    isRec = false;
    printf("Init done.\n");

    img_process_thread = new mythread(this);

    thread = new mythread1;//此处不要指定父对象************************6
    luzhi_thread = new QThread(this);//创建子线程***********************7
    thread->moveToThread(luzhi_thread);//子线性移动到线程通道里******************8

    connect(ui->ImageProcess,&QPushButton::clicked,this,&MainWindow::dealclicked);//按下按钮后执行dealclicked()槽函数

    //当按下窗口右上角x号关闭时触发信号destroyed来实现槽函数stop_img_process_thread关闭线程
    connect(this,&MainWindow::destroyed,this,&MainWindow::stop_img_process_thread);

    //***************************************************************************9下边三个connect
    connect(ui->Start_rec,&QPushButton::clicked,this,&MainWindow::on_Start_rec_clicked);//按钮按下，线程启动,并发射信号
    connect(this,&MainWindow::rec_signal,thread,&mythread1::luzhi);//收到信号调用视频录制函数
    connect(ui->Stop_rec,&QPushButton::clicked,this,&MainWindow::on_Stop_rec_clicked);

}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::dealclicked()
{

    img_process_thread->start();//QThread 的对象通过start()函数调用线程文件中的run()函数
}

void MainWindow::stop_img_process_thread()
{
    //关闭图像处理线程，等待资源回收
    img_process_thread->quit();
    //线程关闭后，需要等待线程处理完手头工作，才能完成资源回收
    img_process_thread->wait();


}
//cv图像格式转换为Qimage函数
QImage MainWindow::Mat2QImage(cv::Mat cvImg)
{
    QImage qImg;
    if(cvImg.channels()==3) //3 通道图像
    {

        cv::cvtColor(cvImg,cvImg,COLOR_BGR2RGB);//opencv里的图像默认是bgr

        //格式转换，将cv图像转换成Qimage格式
        qImg =QImage((const unsigned char*)(cvImg.data),
                    cvImg.cols, cvImg.rows,
                    cvImg.cols*cvImg.channels(),//每行字节数（行跨距）
                    QImage::Format_RGB888);
    }
    else if(cvImg.channels()==1)  //灰度图像
    {
        qImg =QImage((const unsigned char*)(cvImg.data),
                    cvImg.cols,cvImg.rows,
                    cvImg.cols*cvImg.channels(),
                    QImage::Format_Indexed8);
    }
    else
    {
        qImg =QImage((const unsigned char*)(cvImg.data),
                    cvImg.cols,cvImg.rows,
                    cvImg.cols*cvImg.channels(),
                    QImage::Format_RGB888);// format reference
    }

    return qImg;

}

//在控件label里显示图像视频，其实是定时器的槽函数，定时器没发射一次信号便响应一次槽函数，
void MainWindow::nextFrame()
{
    capture >> frame;
    if(!frame.empty())        
    {
        img_process_thread->frame=frame;//把控件里的frame 赋给线程处理函数的frame里 这样使得图像处理视频和控件视频同步

        flip(frame,mirror,1);//翻转图像，1水平 0为垂直翻转 负值则同时翻转
        image = Mat2QImage(mirror);//1.转换图像格式（函数）
        ui->display->setPixmap(QPixmap::fromImage(image));// 2.将QImage格式的image图像转换为QPixmap，在dispaly控件（label）里显示
    }

}
void MainWindow::nextFrame_rec()//********************************************10
{

    thread->frame=frame;
    emit rec_signal();
}

void MainWindow::on_OpenCamera_clicked()
{
    QMessageBox:: StandardButton result =  QMessageBox::information(NULL, "消息提示", "将会打开摄像头！", QMessageBox::Yes | QMessageBox::No|QMessageBox::Abort, QMessageBox::Abort);
        switch (result)
        {
        case QMessageBox::Yes:
            std::cout<<"Yes";
            break;
        case QMessageBox::No:
            std::cout<<"NO";
            return;
            break;
        default:
            return;
            break;
        }

        ui->OpenVedio->setDisabled(true);

        if (capture.isOpened())
            capture.release();     //判断capture是否在使用，若正在使用需要先释放
        capture.open(0);
        if (capture.isOpened())
        {
            ui->OpenCamera->setDisabled(true);
            rate= 15;//;此处应该直接设置帧率（通常15左右），不要使用capture.get(CV_CAP_PROP_FPS)获取不然画面不会动，读取视频文件的时候可以get获取
            capture >> frame;

            if (!frame.empty())
            {

                image = Mat2QImage(frame);

                ui->display->setPixmap(QPixmap::fromImage(image));
                timer = new QTimer(this);
                timer->setInterval(1000/rate);   //为定时器匹配帧率
                connect(timer, SIGNAL(timeout()), this, SLOT(nextFrame()));
                timer->start();
            }
        }
}

void MainWindow::on_OpenVedio_clicked()
{
    ui->OpenCamera->setDisabled(true);
    if (capture.isOpened())
            capture.release();     //decide if capture is already opened; if so,close it
        //读取视频文件的名称并打开
        QString filename =QFileDialog::getOpenFileName(this,tr("Open Video File"),".",tr("Video Files(*.avi *.mp4 *.flv *.mkv)"));
        capture.open(filename.toLocal8Bit().data());

        if (capture.isOpened())
        {
            rate= capture.get(CV_CAP_PROP_FPS);//获取帧率
            capture >> frame;
            if (!frame.empty())
            {
          //1.读取视频文件后更换图像格式并设置label 2.创建定时器匹配帧率，发送信号完成display（标签）视频显示
                image = Mat2QImage(frame);
                ui->display->setPixmap(QPixmap::fromImage(image));
                timer = new QTimer(this);//创建计时器对象
                timer->setInterval(1000/rate);//设置计时器与FPS匹配，1秒钟响应次数和帧率一样，为了用槽函数nextframe在控件里显示每一帧图像
                connect(timer, SIGNAL(timeout()), this, SLOT(nextFrame()));//计时器到了后发送信号，然后进行label控件里的图像显示
                timer->start();//执行 start() 是将当前时间作为计时器的时间
            }
        }

}

void MainWindow::on_CloseCamera_clicked()
{
    capture.release();
    ui->OpenCamera->setEnabled(true);
    ui->OpenVedio->setEnabled(true);


}

void MainWindow::on_Stop_rec_clicked()
{

        isRec = false;
        ui->Start_rec->setEnabled(true);//让开始录制按钮有效
        ui->Stop_rec->setDisabled(true);//让停止按钮失效

        luzhi_thread->quit();
        luzhi_thread->wait();
        thread->deleteLater();

}

void MainWindow::on_Start_rec_clicked()
{

        ui->Stop_rec->setEnabled(true);//让停止录制按钮有效
        ui->Start_rec->setDisabled(true);//让开始按钮失效
        luzhi_thread->start();
        if (capture.isOpened())
        {
            //rate= 30;capture.get(CV_CAP_PROP_FPS);获取帧率
            if (!frame.empty())
            {

          //1.读取视频文件后更换图像格式并设置label 2.创建定时器匹配帧率，发送信号完成display（标签）视频显示
                timer = new QTimer(this);//创建计时器对象
                timer->setInterval(1000/30);//设置计时器与FPS匹配，1秒钟响应次数和帧率一样，为了用槽函数nextframe在控件里显示每一帧图像
                connect(timer, SIGNAL(timeout()), this, SLOT(nextFrame_rec()));//计时器到了后发送信号，然后进行label控件里的图像显示
                timer->start();//执行 start() 是将当前时间作为计时器的时间

            }


        }


}

