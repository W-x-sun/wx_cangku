/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QLabel *display;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout;
    QPushButton *OpenCamera;
    QPushButton *CloseCamera;
    QPushButton *OpenVedio;
    QPushButton *ImageProcess;
    QPushButton *Start_rec;
    QPushButton *Stop_rec;
    QPushButton *quit;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;
    QMenuBar *menuBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(1153, 761);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        display = new QLabel(centralWidget);
        display->setObjectName(QStringLiteral("display"));
        display->setGeometry(QRect(160, 70, 821, 621));
        display->setScaledContents(true);
        layoutWidget = new QWidget(centralWidget);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(200, 20, 701, 30));
        horizontalLayout = new QHBoxLayout(layoutWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        OpenCamera = new QPushButton(layoutWidget);
        OpenCamera->setObjectName(QStringLiteral("OpenCamera"));

        horizontalLayout->addWidget(OpenCamera);

        CloseCamera = new QPushButton(layoutWidget);
        CloseCamera->setObjectName(QStringLiteral("CloseCamera"));

        horizontalLayout->addWidget(CloseCamera);

        OpenVedio = new QPushButton(layoutWidget);
        OpenVedio->setObjectName(QStringLiteral("OpenVedio"));

        horizontalLayout->addWidget(OpenVedio);

        ImageProcess = new QPushButton(layoutWidget);
        ImageProcess->setObjectName(QStringLiteral("ImageProcess"));

        horizontalLayout->addWidget(ImageProcess);

        Start_rec = new QPushButton(layoutWidget);
        Start_rec->setObjectName(QStringLiteral("Start_rec"));

        horizontalLayout->addWidget(Start_rec);

        Stop_rec = new QPushButton(layoutWidget);
        Stop_rec->setObjectName(QStringLiteral("Stop_rec"));

        horizontalLayout->addWidget(Stop_rec);

        quit = new QPushButton(layoutWidget);
        quit->setObjectName(QStringLiteral("quit"));

        horizontalLayout->addWidget(quit);

        MainWindow->setCentralWidget(centralWidget);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1153, 26));
        MainWindow->setMenuBar(menuBar);

        retranslateUi(MainWindow);
        QObject::connect(quit, SIGNAL(clicked()), MainWindow, SLOT(close()));

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", Q_NULLPTR));
        display->setText(QString());
        OpenCamera->setText(QApplication::translate("MainWindow", "\346\211\223\345\274\200\346\221\204\345\203\217\345\244\264", Q_NULLPTR));
        CloseCamera->setText(QApplication::translate("MainWindow", "\345\205\263\351\227\255\346\221\204\345\203\217\345\244\264", Q_NULLPTR));
        OpenVedio->setText(QApplication::translate("MainWindow", "\346\211\223\345\274\200\350\247\206\351\242\221\346\226\207\344\273\266", Q_NULLPTR));
        ImageProcess->setText(QApplication::translate("MainWindow", "\345\233\276\345\203\217\345\244\204\347\220\206", Q_NULLPTR));
        Start_rec->setText(QApplication::translate("MainWindow", "\345\274\200\345\247\213\345\275\225\345\210\266", Q_NULLPTR));
        Stop_rec->setText(QApplication::translate("MainWindow", "\345\201\234\346\255\242\345\275\225\345\210\266", Q_NULLPTR));
        quit->setText(QApplication::translate("MainWindow", "\351\200\200\345\207\272", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
