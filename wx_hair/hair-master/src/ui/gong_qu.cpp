#include "gong_qu.h"
#include "ui_gong_qu.h"

Gong_Qu::Gong_Qu(QWidget *parent) :
    QWidget(parent),
    pro_ui(new Ui::Gong_Qu)
{
    pro_ui->setupUi(this);
}

Gong_Qu::~Gong_Qu()
{
    delete pro_ui;
}
