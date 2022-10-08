#include "fajixian.h"
#include "ui_fajixian.h"

FaJIxian::FaJIxian(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FaJIxian)
{
    ui->setupUi(this);
}

FaJIxian::~FaJIxian()
{
    delete ui;
}
