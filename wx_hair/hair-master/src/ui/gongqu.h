#ifndef GONGQU_H
#define GONGQU_H

#include <QMainWindow>

namespace Ui {
class GongQu;
}

class GongQu : public QMainWindow
{
    Q_OBJECT

public:
    explicit GongQu(QWidget *parent = 0);
    ~GongQu();

private:
    Ui::GongQu *ui;
};

#endif // GONGQU_H
