#ifndef FAJIXIAN_H
#define FAJIXIAN_H

#include <QWidget>

namespace Ui {
class FaJIxian;
}

class FaJIxian : public QWidget
{
    Q_OBJECT

public:
    explicit FaJIxian(QWidget *parent = 0);
    ~FaJIxian();

private:
    Ui::FaJIxian *ui;
};

#endif // FAJIXIAN_H
