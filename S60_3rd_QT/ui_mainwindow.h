/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Fri 28. May 18:09:33 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionFull;
    QAction *actionHalf;
    QAction *actionLow;
    QAction *actionMid;
    QAction *actionHigh;
    QAction *actionFront;
    QAction *actionRear;
    QAction *actionEcho;
    QAction *actionM2M;
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout;
    QMenuBar *menuBar;
    QMenu *menuSize;
    QMenu *menuQuality;
    QMenu *menuCamera;
    QMenu *menuMode;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(268, 298);
        MainWindow->setContextMenuPolicy(Qt::DefaultContextMenu);
        MainWindow->setStyleSheet(QString::fromUtf8(""));
        actionFull = new QAction(MainWindow);
        actionFull->setObjectName(QString::fromUtf8("actionFull"));
        actionFull->setCheckable(true);
        actionHalf = new QAction(MainWindow);
        actionHalf->setObjectName(QString::fromUtf8("actionHalf"));
        actionHalf->setCheckable(true);
        actionLow = new QAction(MainWindow);
        actionLow->setObjectName(QString::fromUtf8("actionLow"));
        actionLow->setCheckable(true);
        actionMid = new QAction(MainWindow);
        actionMid->setObjectName(QString::fromUtf8("actionMid"));
        actionMid->setCheckable(true);
        actionHigh = new QAction(MainWindow);
        actionHigh->setObjectName(QString::fromUtf8("actionHigh"));
        actionHigh->setCheckable(true);
        actionFront = new QAction(MainWindow);
        actionFront->setObjectName(QString::fromUtf8("actionFront"));
        actionFront->setCheckable(true);
        actionRear = new QAction(MainWindow);
        actionRear->setObjectName(QString::fromUtf8("actionRear"));
        actionRear->setCheckable(true);
        actionEcho = new QAction(MainWindow);
        actionEcho->setObjectName(QString::fromUtf8("actionEcho"));
        actionEcho->setCheckable(true);
        actionM2M = new QAction(MainWindow);
        actionM2M->setObjectName(QString::fromUtf8("actionM2M"));
        actionM2M->setCheckable(true);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        centralWidget->setEnabled(true);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(centralWidget->sizePolicy().hasHeightForWidth());
        centralWidget->setSizePolicy(sizePolicy);
        horizontalLayout = new QHBoxLayout(centralWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 268, 20));
        menuSize = new QMenu(menuBar);
        menuSize->setObjectName(QString::fromUtf8("menuSize"));
        menuQuality = new QMenu(menuBar);
        menuQuality->setObjectName(QString::fromUtf8("menuQuality"));
        menuCamera = new QMenu(menuBar);
        menuCamera->setObjectName(QString::fromUtf8("menuCamera"));
        menuMode = new QMenu(menuBar);
        menuMode->setObjectName(QString::fromUtf8("menuMode"));
        MainWindow->setMenuBar(menuBar);

        menuBar->addAction(menuSize->menuAction());
        menuBar->addAction(menuQuality->menuAction());
        menuBar->addAction(menuCamera->menuAction());
        menuBar->addAction(menuMode->menuAction());
        menuSize->addAction(actionFull);
        menuSize->addAction(actionHalf);
        menuQuality->addAction(actionLow);
        menuQuality->addAction(actionMid);
        menuQuality->addAction(actionHigh);
        menuCamera->addAction(actionFront);
        menuCamera->addAction(actionRear);
        menuMode->addAction(actionEcho);
        menuMode->addAction(actionM2M);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MobiVu", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        MainWindow->setStatusTip(QString());
#endif // QT_NO_STATUSTIP
        actionFull->setText(QApplication::translate("MainWindow", "Full", 0, QApplication::UnicodeUTF8));
        actionHalf->setText(QApplication::translate("MainWindow", "Half", 0, QApplication::UnicodeUTF8));
        actionLow->setText(QApplication::translate("MainWindow", "Low", 0, QApplication::UnicodeUTF8));
        actionMid->setText(QApplication::translate("MainWindow", "Mid", 0, QApplication::UnicodeUTF8));
        actionHigh->setText(QApplication::translate("MainWindow", "High", 0, QApplication::UnicodeUTF8));
        actionFront->setText(QApplication::translate("MainWindow", "Front", 0, QApplication::UnicodeUTF8));
        actionRear->setText(QApplication::translate("MainWindow", "Rear", 0, QApplication::UnicodeUTF8));
        actionEcho->setText(QApplication::translate("MainWindow", "Echo", 0, QApplication::UnicodeUTF8));
        actionM2M->setText(QApplication::translate("MainWindow", "M2M", 0, QApplication::UnicodeUTF8));
        menuSize->setTitle(QApplication::translate("MainWindow", "Size", 0, QApplication::UnicodeUTF8));
        menuQuality->setTitle(QApplication::translate("MainWindow", "Quality", 0, QApplication::UnicodeUTF8));
        menuCamera->setTitle(QApplication::translate("MainWindow", "Camera", 0, QApplication::UnicodeUTF8));
        menuMode->setTitle(QApplication::translate("MainWindow", "Mode", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
