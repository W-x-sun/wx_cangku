#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include "glwidget.h"

namespace Ui {
    class MainWindow;
}

class HairInterface;//ui交互 类

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *m_ui;//主窗口
    HairInterface *m_hairInterface;//操作对象
    GLWidget *m_glWidget;//绘制仿真模型的窗口

};

#endif // MAINWINDOW_H
