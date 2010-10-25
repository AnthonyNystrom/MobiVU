#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "threadudp.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void Start();
    void Pause();
    void timerEvent(QTimerEvent *event);
protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;
    QList<ThreadUDP*> _listThreads;
    bool _bStarted;
    void ClearMem();
    int _idTimer;
    int _iTimeCnt;
    bool _bPause;
};

#endif // MAINWINDOW_H
