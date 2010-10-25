#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "server.h"

#include <QTreeWidgetItem>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

//    Server *server9051, *server9052;
    Server *server[120];

protected:
    void changeEvent(QEvent *e);
    void getCpuLoad(int* user, int* nice, int* kernel, int* idle);

private:
    Ui::MainWindow *ui;
    int user, nice, kernel, idle;

private slots:
    void on_pushButton_clicked();
    void update();
    void showItemDetailsSlot(QTreeWidgetItem* item, int column);
};

#endif // MAINWINDOW_H
