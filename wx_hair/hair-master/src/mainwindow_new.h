#ifndef MAINWINDOW_NEW_H
#define MAINWINDOW_NEW_H

#include <QMainWindow>
#include "gong_qu.h"
#include "sim_fun.h"
#include "fajixian.h"

#include "glwidget.h"
#include <QList>
#include "hairinterface.h"

namespace Ui {
class MainWindow_NEW;
}
class HairInterface;
class GLWidget;

class MainWindow_NEW : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow_NEW(QWidget *parent = 0);
    ~MainWindow_NEW();

private:
    Ui::MainWindow_NEW *m_ui;//主窗口
    //按钮子窗口
    Gong_Qu *gong_qu;//供区 子窗口
    Sim_Fun *sim_fun;//仿真效果 子窗口
    FaJIxian *fajixian;//发际线 子窗口


    GLWidget *m_glWidget;//绘制仿真模型的窗口

private slots:       
    void gong_qu_sonWidget();//打开供区 子窗口
    void sim_resuit_sonWidget();//打开仿真效果功能 子窗口
    void clear_all_design();//清空所有设计 对话框
    void fajixian_sonWidget();//打开发际线 子窗口
};

#endif // MAINWINDOW_NEW_H
