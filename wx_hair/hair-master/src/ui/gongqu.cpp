#include "gongqu.h"
#include "ui_gongqu.h"

GongQu::GongQu(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GongQu)
{
    ui->setupUi(this);
}

GongQu::~GongQu()
{
    delete ui;
}
