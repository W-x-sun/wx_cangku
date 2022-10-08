#include <QApplication>
#include "mainwindow.h"
#include "mainwindow_new.h"
#include "gongqu.h"
#include "glw.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    //主窗口
//    MainWindow_NEW w_new;
//    w_new.show();
//    w_new.setWindowTitle("植发设计");





    return a.exec();
}
