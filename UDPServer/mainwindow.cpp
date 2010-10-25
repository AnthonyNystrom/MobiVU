#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QTextStream>
#include <QLatin1String>

#include <QStringListModel>
#include <QTreeWidgetItem>

//#include "Platform.h"


#include "Model.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->tableWidget->setRowCount(120);

    for (int i=0; i<120; i++) {
        server[i] = new Server(9050+i, (i>99), this);
        server[i]->run();

        for (int ii=0; ii<7; ii++) {
            QTableWidgetItem *newItem;
            if (ii==0) newItem = new QTableWidgetItem(tr("%1").arg(9050+i));
            else newItem = new QTableWidgetItem(tr("%1").arg(0));
            ui->tableWidget->setItem(i, ii, newItem);
        }
    }

    // Faccio partire un timer per l'aggiornamento dei dati a video
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(1000);

    statusBar()->showMessage("UDP Server started...");
}

MainWindow::~MainWindow()
{
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


//
// Tasto azzeramento conteggi
//
void MainWindow::on_pushButton_clicked()
{
    for (int i=0; i<120; i++) {
        server[i]->resetCounters();
    }
}


//
// Aggiornamento del dialogo
//
void MainWindow::update()
{
    for (int i=0; i<120; i++) {
        for (int ii=1; ii<7; ii++) {

            QTableWidgetItem *Item = ui->tableWidget->item(i, ii);

            switch(ii){
            case 1:
                Item->setText(tr("%1").arg( server[i]->pacchettiRicevuti));
                break;
            case 2:
                Item->setText(tr("%1").arg( server[i]->pacchettiPersi));
                break;
            case 3:
                Item->setText(tr("%1").arg( server[i]->lastCounterReceived));
                break;
            case 4:
                Item->setText(tr("%1").arg( server[i]->clientsList[0].hostAddress.toString()));
                break;
            case 5:
                Item->setText(tr("%1").arg( server[i]->clientsList[1].hostAddress.toString()));
                break;
            case 6:
                Item->setText(tr("%1").arg( server[i]->clientsList[2].hostAddress.toString()));
                break;
            default:
                Item->setText(tr("%1").arg("??"));
                break;
            }
        }
    }

    int porta = ui->lineEdit->text().toInt();
    if ((porta >= 9050) && (porta <= (9050+120))) {
	// Visualizzo i dettagli della porta che ho selezionato
        ui->label_2->setText(QString::number(server[porta-9050]->packetsReceived));
        ui->label_5->setText(QString::number(server[porta-9050]->kbps,'f',1));
        ui->kbpsLab->setText(QString::number(server[porta-9050]->kbps*8,'f',1));

        // Aggiorno jitter ecc...
        ui->minJittLabel9051->setText(QString::number(server[porta-9050]->minJitt,'f',1));
        ui->avgJittLabel9051->setText(QString::number(server[porta-9050]->avgJitt,'f',1));
	ui->maxJittLabel9051->setText(QString::number(server[porta-9050]->maxJitt,'f',1));

        ui->ip1Label->setText(server[porta-9050]->clientsList[0].hostAddress.toString());
	ui->ip2Label->setText(server[porta-9050]->clientsList[1].hostAddress.toString());
	
        ui->switchPortLabel->setText(QString::number(server[porta-9050]->switchPortCounter));
	// Segnalo che ho una conversazione in corso
	// TODO: Segnalo che ho una conversazione in corso

        ui->lastSenderAddressLabel->setText(server[porta-9050]->lastSenderAddress.toString ());
	ui->lastSenderPortLabel->setText(QString::number(server[porta-9050]->lastSenderPort));
    }
}



void MainWindow::showItemDetailsSlot(QTreeWidgetItem* item, int column)
{
    qDebug() << item->text(0);
}
