#include "mythread.h"

mythread::mythread(QObject *parent) : QThread(parent)
{

}



void mythread::on_ImageProcess_clicked(cv::Mat frame)
{


    height = capture.get(CV_CAP_PROP_FRAME_HEIGHT); // 代表图像尺寸 road：480
    width = capture.get(CV_CAP_PROP_FRAME_WIDTH);  // 代表图像尺寸 road：856
    count = capture.get(CV_CAP_PROP_FRAME_COUNT);

    picture_flag = true;
    //cv::Mat cannyImg ;
    while(picture_flag)
    {

        if(frame.empty())
        {
            std::cout<<"图像加载失败!"<<std::endl;
            picture_flag = false;
            return;
        }
        //下边做图像处理的部分

        detect_line(frame);


        waitKey(10);
        if(getWindowProperty("display_dst",WND_PROP_AUTOSIZE) != 1)
                    break;

    }
     capture.release();
     destroyAllWindows();
}



Mat mythread::fitLines(Mat &img)
{

    Mat img_fitLines = Mat::zeros(img.size(), CV_8UC3);//CV_8UC3为三通道

    int height = img.rows;
    int width = img.cols;

    int h_center = height / 2;
    int w_center = width / 2;

    vector<Point> leftLine;//左容器里放左车道线的点
    vector<Point> rightLine;

    Point left_start_last, left_end_last;//定义的变量
    Point left_start, left_end;
    int x2_last;

    //左车道线
    for (int i = 100; i < w_center; i++)       //100-428(左)
    {
        for (int j = h_center; j < height; j++)  //240-480(下)
        {
            if (img.at<uchar>(j, i) == 255) //如果左下角的部分区域内的车道线为白色线
                    leftLine.push_back(Point(i, j));//将检测到的每一个白色点拼接成一条线(实际上是无数点的集合，点集)
            //push_back(参数)函数的用法，函数将一个新的元素加到leftline的最后面，加入的是参数值

        }
    }
    if (leftLine.size() > 2)//必须大于二维，否则没有意义
    {
        Vec4f left_para; //直线拟合输出参数
        Point point_l; //直线上的一点

        fitLine(leftLine, left_para, 1, 0, 0.01, 0.01);  //直线拟合

        /*cv::InputArray points, // 二维点的数组或vector  (需要拟合的点集)
        cv::OutputArray line, // 输出直线
        int distType, // 距离类型，拟合直线时，要使输入点到拟合直线的距离和最小化
        double param, // 距离参数
        double reps, // 径向的精度参数
        double aeps // 角度精度参数
        */
        double kl = left_para[1] / left_para[0];  //直线斜率

        point_l.x = left_para[2];
        point_l.y = left_para[3];

        int y1 = height / 2 + 60;
        int x1 = (y1 - point_l.y) / kl + point_l.x;

        int y2 = height - 40;
        int x2 = (y2 - point_l.y) / kl + point_l.x;

        /*cout << x2_last << endl;
        if ((x2_last!=0) && (abs(x2 - x2_last) > 30))
        {
            cout << "测试" << endl;
            x2 = x2_last;
        }*/

        left_start = Point(x1, y1);//左线起点
        left_end = Point(x2, y2);//左线终点

        line(img_fitLines, left_start, left_end, Scalar(0, 255, 0), 8, 8, 0);
        /*参数如下：
                img: 要绘制线段的图像。
                pt1: 线段的起点。
                pt2: 线段的终点。
                color: 线段的颜色，通过一个Scalar对象定义。B ,G ,R
                thickness: 线条的宽度。
                lineType: 线段的类型。可以取值8， 4， 和CV_AA， 分别代表8邻接连接线，4邻接连接线和反锯齿连接线。默认值为8邻接。为了获得更好地效果可以选用CV_AA(采用了高斯滤波)。
                shift: 坐标点小数点位数。
        */

        left_start_last = left_start;
        left_end_last = left_end;
        x2_last = x2;
    }
    else
    {
        line(img_fitLines, left_start_last, left_end_last, Scalar(0, 225, 0), 8, 8, 0);
    }

    //右车道线
    for (int i = w_center; i < width; i++)   //428-856(右)
    {
        for (int j = h_center; j < height; j++)  //240-480（下）
        {
            if (img.at<uchar>(j, i) == 255)
                rightLine.push_back(Point(i, j));
        }
    }

    if (rightLine.size() > 2)
    {
        Point right_start = rightLine[0];
        Point right_end = rightLine[rightLine.size() - 1];

        line(img_fitLines, right_start, right_end, Scalar(0, 255, 0), 8, 8, 0);
    }

    return img_fitLines;
}

void mythread::detect_line(Mat &yuantu)
{
    Mat gray, binary;
    cvtColor(yuantu, gray, COLOR_BGR2GRAY); //转灰度图
    Canny(gray, binary, 140, 300); //边缘检测
   // imshow("边缘检测", binary);


    /********************过滤掉天空与旁景色********************/       //左上角为坐标原点
    for (int i = 0; i < (gray.rows /2 + 60 ); i++)//过滤天空，对应关系row == heigh，col == width
    {
        for (int j = 0; j < gray.cols; j++)
        {
            binary.at<uchar>(i, j) = 0;//Mat类中的at方法对于获取图像矩阵某点的RGB值或者改变某点的值很方便
                                      //            =0表示让天空区域都变成黑色（就是过滤）
        }
    }


    for (int i = 470; i < gray.rows; i++)   //过滤旁景色470-480(图像底部车前部)，对应关系row == heigh，col == width
    {
        for (int j = 0; j < gray.cols; j++)
        {
            binary.at<uchar>(i, j) = 0;
        }
    }
    //imshow("提取感兴趣区域后", binary);

    /*********************************************************/

        //寻找轮廓，contours被定义成二维浮点型向量，这里面将来会存储找到的边界的（x,y）坐标。
    vector<vector<Point>> contours;  //（vector容器里面放了一个vector容器，子容器里放点）
                                    //向量中是若干个点的集合，每一个集合代表一个轮廓，包含若干个点



    findContours(binary, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE); //提取轮廓放到contours容器里，且仅保存轮廓的拐点信息
    //CV_CHAIN_APPROX_SIMPLE 仅保存轮廓的拐点信息，把所有轮廓拐点处的点保存入contours向量内，拐点与拐点之间直线段上的信息点不予保留
    //CV_RETR_EXTERNAL只检测最外围轮廓，包含在外围轮廓内的内围轮廓被忽略


    Mat img_output = Mat::zeros(gray.size(), gray.type());

    /********************************************************
        1.Mat矩阵的类型作用域里的zeros函数：type的命名格式为CV_(位数)+(数据类型)+(通道数)例如：Mat Z = Mat::zeros(3, 3, CV_8UC1);
        的效果是：生成一个3*3的矩阵，取值为CV_8UC1
        2.首先size是一个结构体，定义了Mat矩阵内数据的分布形式，数值上有关系式：
         image.size().width==image.cols;        image.size().height==image.rows
        3.img_output为mat的一个矩阵类型
    ***********************************************************************/

    /*****************************轮廓分析（筛选）***************************
        1.排除轮廓（长度小于5、面积不足10的，同时矩形的高不能太大）
        2.排除最小外切矩形角度太小的或者太大的（20，84）
        3.排除椭圆拟合角度过小的
    ***********************************************************************/
    for (int i = 0; i < contours.size(); i++)
    {
        //计算每个轮廓的面积和周长
        double length = arcLength(contours[i], true);
        double area = contourArea(contours[i]);
        //cout << "轮廓" << i << "的周长是：" << length << " " << "面积是" << area << endl;

        //得到包覆此轮廓的最小正矩形
        Rect rect;
        rect = boundingRect(contours[i]);

        //最小包围矩形（斜）
        RotatedRect mAR = minAreaRect(contours[i]);
        double angle = abs(mAR.angle);  //倾斜角度

        if (length < 5.0 || area < 10.0)
            continue;
        //if (rect.y > gray.rows - 50)
            //continue;
        if (angle > 84.0 || angle < 20.0)  //去掉角度大的边线
            continue;

        drawContours(img_output, contours, i, Scalar(255), 2, 8);//绘制轮廓函数，各个参数的意义：
        /*****************************参数意义（筛选）***************************
            InputOutputArray image,//要绘制轮廓的图像
            InputArrayOfArrays contours,//所有输入的轮廓，每个轮廓被保存成一个point向量
            int contourIdx,//指定要绘制轮廓的编号，如果是负数，则绘制所有的轮廓
            const Scalar& color,//绘制轮廓所用的颜色，由于是单通道，数值只表示白色亮度强度
            int thickness = 1, //绘制轮廓的线的粗细，如果是负数，则轮廓内部被填充
            int lineType = 8, / 绘制轮廓的线的连通性
    ***********************************************************************/


        //imshow("排除部分轮廓后", img_output);
    }

    cv::Mat roadLines;
    cv::Mat dst;
    roadLines = fitLines(img_output);
    addWeighted(yuantu, 0.9, roadLines, 0.5, 0, dst);//将原图和车道线图片混合到一张最终图上
    /*参数如下：void cv::addWeighted(
                InputArray src1, //图片1
                double alpha,    //混合比例0.9
                InputArray src2, //图片2，（注意：其大小和通道与1相同，都是三通道的）
                double beta,     //混合比例0.5
                double gamma,    //偏差，每次计算加上该值
                OutputArray dst, //输出图片，其大小和通道与输入相同
                int dtype = -1   //输出数组的可选深度；当两个输入数组的深度相同时，可以将dtype设置为-1，这等效于src1.depth()。
                                        )

    */
    namedWindow("display_dst",WINDOW_AUTOSIZE);
    imshow("display_dst", dst);
}

void mythread::run()
{
    on_ImageProcess_clicked(frame);
    //emit over();//复杂函数结束后发出信号
}
