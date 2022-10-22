#include "ServerWidget.h"
#include "ui_ServerWidget.h"

#include <QFileDialog>
#pragma execution_character_set("utf-8")

ServerWidget::ServerWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ServerWidget)
{
    ui->setupUi(this);

    thread = new QThread(this);
    operate = new ServerOperate;
    operate->moveToThread(thread);
    //退出时释放线程
    connect(thread,&QThread::finished,operate,&ServerOperate::deleteLater);
    //服务端监听（按钮）开关
    connect(ui->btnListen,&QPushButton::clicked,[this]{
        if(operate->isListening()){//如果正在监听
            emit dislisten();//让监听按钮变为dislisten
        }else{
            emit listen(ui->editAddress->text(),ui->editPort->text().toUShort());
        }
    });
    connect(this,&ServerWidget::listen,operate,&ServerOperate::listen);//触发槽函数完成监听
    connect(this,&ServerWidget::dislisten,operate,&ServerOperate::dislisten);//触发槽函数（关闭监听服务）
    //服务器监听状态改变（若正监听，则把一些按钮和文本框 使能关闭）
    connect(operate,&ServerOperate::listenStateChanged,this,[this](bool isListen){
        ui->btnListen->setText(isListen?"Dislisten":"Listen");
        ui->editAddress->setEnabled(!isListen);
        ui->editPort->setEnabled(!isListen);
        ui->editPath->setEnabled(!isListen);
        ui->btnSelect->setEnabled(!isListen);
    });
    //选择要保存文件的路径
    connect(ui->btnSelect,&QPushButton::clicked,[=]{
        const QString dir_path=QFileDialog::getExistingDirectory(this);
        ui->editPath->setText(dir_path);
    });
    connect(ui->editPath,&QLineEdit::textChanged,operate,&ServerOperate::setFilePath);
    //取消文件传输
    connect(ui->btnCancel,&QPushButton::clicked,operate,&ServerOperate::cancelFileTransfer);
    //日志（通过信号槽的方式：将信号发来的日志内容显示到文本框里）
    connect(operate,&ServerOperate::logMessage,this,[=](const QString &msg){
        ui->textEdit->append(msg);
    });
    //进度条
    connect(operate,&ServerOperate::progressChanged,ui->progressBar,&QProgressBar::setValue);

    //启动线程
    thread->start();
}

ServerWidget::~ServerWidget()
{
    thread->quit();
    thread->wait();
    delete ui;
}


