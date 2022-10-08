#include "mainwindow_new.h"
#include "ui_mainwindow_new.h"

#include "hairinterface.h"
#include <QSettings>

#include <QMessageBox>

MainWindow_NEW::MainWindow_NEW(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::MainWindow_NEW)
{
    m_ui->setupUi(this);

    //１．opengl的相关设置
    QGLFormat qglFormat;
    qglFormat.setVersion(4,2);
    qglFormat.setProfile(QGLFormat::CoreProfile);
    m_ui->setupUi(this);

    //３．实例化对象(绘制模型的窗口)
//   m_glWidget = new GLWidget(qglFormat, this);//m_glWidget是绘制的窗口，调用了自己重载的

  // m_sceneWidget = new SceneWidget(m_glWidget, mesh);//m_sceneWidget是渲染的主窗口
  // m_ui->gridLayout_2->addWidget(m_sceneWidget);
//   m_ui->gridLayout_3->addWidget(m_glWidget);

    //三个子窗口（供区、仿真、发际线）
    gong_qu = new Gong_Qu();
    sim_fun = new Sim_Fun();
    fajixian = new FaJIxian();


    /**************相应按钮的子窗口触发**************/
    connect(m_ui->pro_pushbutton, SIGNAL(clicked()), this, SLOT(gong_qu_sonWidget()));//供区
    connect(m_ui->sim_pushbuttion, SIGNAL(clicked()), this, SLOT(sim_resuit_sonWidget()));//仿真功能
    connect(m_ui->clear_pushButton, SIGNAL(clicked()), this, SLOT(clear_all_design()));//清空
    connect(m_ui->fjx_pushButton, SIGNAL(clicked()), this, SLOT(fajixian_sonWidget()));
}

MainWindow_NEW::~MainWindow_NEW()
{
    delete m_ui;
    delete gong_qu;
    delete sim_fun;
    delete fajixian;

   // delete m_hairInterface2;
//    delete m_glWidget;
}

/**************相应按钮的子窗口触发  槽函数实现**************/
////打开供区勾画子窗口
void MainWindow_NEW::gong_qu_sonWidget(){
    //打开对应窗口
    gong_qu->setWindowTitle("供区勾画方案设计");
    gong_qu->show();
    gong_qu->activateWindow();

    //其他处理

}

////打开仿真效果功能 子窗口
void MainWindow_NEW::sim_resuit_sonWidget(){
    //打开对应窗口
    sim_fun->setWindowTitle("仿真效果功能选取");
    sim_fun->show();
    sim_fun->activateWindow();
    //其他处理

}
//清空所有设置，恢复仿真的初始状态
void MainWindow_NEW::clear_all_design(){

    //弹出对话框（确认按钮点击后生效，取消按钮点击后关闭该子窗口）
    QMessageBox::question(NULL, "清空", "是否清空所有设计？", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

}

void MainWindow_NEW::fajixian_sonWidget(){
    //打开窗口
    fajixian->setWindowTitle("发际线画笔设计");
    fajixian->show();
    fajixian->activateWindow();
    //其他处理

}
