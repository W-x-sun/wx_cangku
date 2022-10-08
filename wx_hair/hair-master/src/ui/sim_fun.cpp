#include "sim_fun.h"
#include "ui_sim_fun.h"

Sim_Fun::Sim_Fun(QWidget *parent) :
    QWidget(parent),
    sim_ui(new Ui::Sim_Fun)
{
    sim_ui->setupUi(this);

    //仿真窗口里的sim_ui交互接口
    connect(sim_ui->HairlongSlider, SIGNAL(valueChanged(int)), this, SLOT(setHairLongth_label(int)));

}

Sim_Fun::~Sim_Fun()
{
    delete sim_ui;
}

//对应槽
//slider与label同步
void Sim_Fun::setHairLongth_label(int length){
    sim_ui->HairlongSlider->setMinimum(0); // 设置进度条的最小值
    sim_ui->HairlongSlider->setMaximum(20); // 设置进度条的最大值
    sim_ui->Hairlong_label->setText(QString::number(length));//滑动条数值改变 同步到对应文本框
}


