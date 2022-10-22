#include "mainwindow.h"
#include <QApplication>
#include <QLabel>
#include "mythread.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle(QStringLiteral("车道线检测"));//设置主窗口名称
    w.show();

    return a.exec();
}
