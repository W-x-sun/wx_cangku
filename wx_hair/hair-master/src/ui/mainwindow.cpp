#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "hairinterface.h"
#include <QSettings>
#include <QGridLayout>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow)
{
    //１．opengl的相关设置
    QGLFormat qglFormat;
    qglFormat.setVersion(4,2);
    qglFormat.setProfile(QGLFormat::CoreProfile);
    m_ui->setupUi(this);
    //２．布局
    QGridLayout *gridLayout = new QGridLayout(m_ui->centralWidget);

    //３．(模型操作对象、绘制模型的窗口)
    m_hairInterface = new HairInterface(m_ui);//m_hairInterface，可以调用其接口API（hairInterface.cpp）进行发型仿真设计
    m_glWidget = new GLWidget(qglFormat, m_hairInterface, this);//m_glWidget　是绘制仿真模型的gl绘制窗口

    //布局（添加了m_glWidget）
    gridLayout->addWidget(m_glWidget, 0, 1);
}

MainWindow::~MainWindow()
{
    delete m_ui;
    delete m_hairInterface;
    delete m_glWidget;
}
