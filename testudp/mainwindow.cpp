#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _bPause = _bStarted = false;
}

MainWindow::~MainWindow()
{
    if (_bStarted)
        Start();
    ClearMem();
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::ClearMem(){
    for (int i=0; i<_listThreads.count(); i++){
        _listThreads.at(i)->StopWrite();
        delete _listThreads.at(i);
    }
    _listThreads.clear();
}

void MainWindow::Pause(){
    _bPause = !_bPause;
    for (int i=0; i<_listThreads.count(); i++){
        _listThreads.at(i)->Pause(_bPause);
    }
    if (_bPause){
        ui->butPause->setText("Go");
        timerEvent(NULL);
    }
    else{
        ui->butPause->setText("Pause");
    }
}

void MainWindow::Start(){
    ui->pushButton->setEnabled(false);
    ui->butPause->setText("Pause");
    if (!_bStarted){
        bool ok;
        int iStartPort = ui->txtFromPort->text().toInt(&ok, 10);
        int iNPorts    = ui->lineEdit->text().toInt(&ok,10);
        ClearMem();
        
        int i=0;
        for (i=0;i<iNPorts;i++){
            ThreadUDP *thudp = new ThreadUDP();
            _listThreads.append(thudp);
            thudp->InitSocket(iStartPort+i);
        }
        ui->tableWidget->setRowCount(_listThreads.count());
        //ui->tableWidget->clear();
        for (int i=0; i<_listThreads.count(); i++){
            for (int ii=0;ii<6;ii++){
                QTableWidgetItem *newItem;
                switch(ii){
                case 0:
                    newItem = new QTableWidgetItem(tr("%1").arg(_listThreads.at(i)->_iNCampTx));
                    break;
                case 1:
                    newItem = new QTableWidgetItem(tr("%1").arg(_listThreads.at(i)->_iNCampRx));
                    break;
                case 2:
                    newItem = new QTableWidgetItem(tr("%1").arg(_listThreads.at(i)->_iMin));
                    break;
                case 3:
                    newItem = new QTableWidgetItem(tr("%1").arg(_listThreads.at(i)->_iMax));
                    break;
                case 4:
                    newItem = new QTableWidgetItem(tr("%1").arg(_listThreads.at(i)->_iAvg));
                    break;
                case 5:
                    newItem = new QTableWidgetItem(tr("%1").arg(_listThreads.at(i)->_iLost));
                    break;
                }
                ui->tableWidget->setItem(i, ii, newItem);
            }
        }
        ui->pushButton->setText("Stop");
        _iTimeCnt=0;
        _idTimer = startTimer(1000);
    }
    else{
        killTimer(_idTimer);
        for (int i=0; i<_listThreads.count(); i++){
            _listThreads.at(i)->StopWrite();            
        }        
        ui->pushButton->setText("Start");

    }    
    _bStarted = !_bStarted;
    ui->pushButton->setEnabled(true);
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    if (!_bPause){
        ui->labTime->setText(tr("%1 sec.").arg(_iTimeCnt));
        _iTimeCnt++;
    }
    for (int i=0; i<_listThreads.count(); i++){
        for (int ii=0;ii<6;ii++){
            QTableWidgetItem *Item = ui->tableWidget->item(i, ii);

            switch(ii){
            case 0:
                Item->setText(tr("%1").arg(_listThreads.at(i)->_iNCampTx));
                break;
            case 1:
                Item->setText(tr("%1").arg(_listThreads.at(i)->_iNCampRx));
                break;
            case 2:
                Item->setText(tr("%1").arg(_listThreads.at(i)->_iMin));
                break;
            case 3:
                Item->setText(tr("%1").arg(_listThreads.at(i)->_iMax));
                break;
            case 4:
                Item->setText(tr("%1").arg(_listThreads.at(i)->_iAvg));
                break;
            case 5:
                Item->setText(tr("%1").arg(_listThreads.at(i)->_iLost));
                break;
            }            
        }
    }
}
