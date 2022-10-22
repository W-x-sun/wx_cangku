#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QSerialPort>        //提供访问串口的功能
#include <QMainWindow>
#pragma execution_character_set("utf-8")
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QSerialPort serial;
private slots:
void serialPort_readyRead();
void on_searchButton_clicked();
void on_openButton_clicked();
void on_sendButton_clicked();
void on_clearButton_clicked();
};

#endif // MAINWINDOW_H
