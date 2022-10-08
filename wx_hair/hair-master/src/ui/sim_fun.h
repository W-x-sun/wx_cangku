#ifndef SIM_FUN_H
#define SIM_FUN_H

#include <QWidget>

namespace Ui {
class Sim_Fun;
}

class Sim_Fun : public QWidget
{
    Q_OBJECT

public:
    explicit Sim_Fun(QWidget *parent = 0);
    ~Sim_Fun();

private:
    Ui::Sim_Fun *sim_ui;

private slots:
   void setHairLongth_label(int);


};

#endif // SIM_FUN_H
