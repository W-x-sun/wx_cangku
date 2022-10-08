#ifndef GONG_QU_H
#define GONG_QU_H

#include <QWidget>

namespace Ui {
class Gong_Qu;
}

class Gong_Qu : public QWidget
{
    Q_OBJECT

public:
    explicit Gong_Qu(QWidget *parent = 0);
    ~Gong_Qu();

private:
    Ui::Gong_Qu *pro_ui;
};

#endif // GONG_QU_H
