#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QtMultimedia>
#include <QFile>
#include <QDebug>
#include <f32file.h>
#include <s32file.h>
#include <utf.h>
#include <pathinfo.h>
#include <in_sock.h>
#include <QThread>
#include "MySleep.h"

MainWindow *iAppUi;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    _iCntLog = 0;
    _bPause  = false;
    _bClearLog = true;
    iAppUi = this;
    _bClearLog=true;
    ui->setupUi(this);
    //createMyMenu();

    _udp = new ThreadUDP(this);
    on_actionEcho_triggered();

    _myCamera = new MyCamera(this);
    _myCamera->SetCamera(true);
    on_actionMid_triggered();

    /*on_actionMid_triggered();
    on_actionRear_triggered();*/

    _decoder    = new CFFDecode();
    _iCodec     = CODEC_ID_H263;
    int iScaleW = _VIDEO_SCALE_FULL_W;
    int iScaleH = _VIDEO_SCALE_FULL_H;
    ui->actionFull->setChecked(true);
    if (!_decoder->Init(_iCodec, VIDEO_W, VIDEO_H, iScaleW, iScaleH, 1000, 0, NULL))
    {   delete _decoder;
        _decoder = NULL;
        LogInfo("Decoder Init Failed");
    }

    _FrameImage = new CFrame(ui->centralWidget);
    _FrameImage->setObjectName(QString::fromUtf8("FrameImage"));
    _FrameImage->SetSize(iScaleW, iScaleH);
    StartCamera(true);
    QObject::connect(_decoder, SIGNAL(Refresh(unsigned char*)), this, SLOT(OnRefresh(unsigned char*)));
}

MainWindow::~MainWindow()
{
    StartCamera(false);

    delete _FrameImage;
    delete menu_helpAction;
    delete menu_aboutAction;
    delete menu_versionAction;
    delete menu_exitAction;
    delete ui;
    delete _timer;
    delete _udp;
    delete _decoder;    
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

void MainWindow::LogInfo(char *pBuf, ...){

    char buffer[512];
    va_list args;
    va_start (args, pBuf);
    vsprintf (buffer,pBuf, args);
    va_end (args);
    qDebug()<<"LogInfo "<<_iCntLog<<" :"<<buffer;
    _iCntLog++;

    return;

    QFile file("C:/Data/mobivu.log");
    /*if (_bClearLog){
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return;
        _bClearLog=false;
        file.close();
    }*/

    if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        return;


    QTextStream out(&file);
    out << buffer << "\r\n";
    file.close();
    return;
}


void MainWindow::DisplayFrame(CFbsBitmap* aFrame)
{

}
void MainWindow::SendAudioFrame(TDesC8& aBuffer){
    if (_udp){        
        unsigned char *p = (unsigned char*)aBuffer.Ptr();
        _udp->WriteAudio(p, aBuffer.Length());
    }
    else
        LogInfo("MainWindow::SendVideoFrame: udp no init");
}


void MainWindow::createMyMenu()
{
    //add menu item help to menu
    menu_helpAction = new QAction(tr("Help"), this);
    menuBar()->addAction(menu_helpAction);
    connect(menu_helpAction, SIGNAL(triggered()), this, SLOT(helpAction()));

    //add menu item help to About
    menu_aboutAction = new QAction(tr("About"), this);
    menuBar()->addAction(menu_aboutAction);
    connect(menu_aboutAction, SIGNAL(triggered()), this, SLOT(aboutAction()));

}

//Handle action for help menu.
void MainWindow::helpAction()
{

}
//Handle action for help About.
void MainWindow::aboutAction()
{

}


void MainWindow::DebugDecoder()
{
    QFile f ("c:/video.h263");
    QMessageBox msgBox;
    if (!f.open (QIODevice::ReadOnly | QIODevice::Text))
        msgBox.setText("fail to open file");
    else
        msgBox.setText("file opend");
    f.close();
    msgBox.exec();
}
void MainWindow::DebugEncoder()
{

}

void MainWindow::StartCamera(bool bEnable)
{
    if (_myCamera)
    {
        if (bEnable){
            //_myCamera->SetSize(width(), height());
            _myCamera->StartCamera();
        }
        else{
            _myCamera->StopCamera();

        }

    }
    /*if(_Camera){
        if (bEnable){            
            TRect rc;
            rc.SetHeight(height());
            rc.SetWidth(width());
            _Camera->SetSizeRect(rc);
            _Camera->StartCamera();
            LogInfo("StartCamera");
        }
        else if (!bEnable)
        {   LogInfo("StopCamera");
            _Camera->StartCapture(EFalse);
            _Camera->StopCamera();
        }
    }*/
}


void MainWindow::SendVideoFrame(unsigned char *p, int iSize)
{   if (_udp)
        _udp->WriteVideo(p, iSize);
    else
        LogInfo("MainWindow::SendVideoFrame: udp no init");
}
void MainWindow::DecodeAndShow(unsigned char *p, int iSize)
{
    if (_decoder && !_bPause){

        if (_decoder->DecodePic(p, (long)iSize))
        {
            //_FrameImage->DrawVideo(true);            
        }
        else
            LogInfo("DecodeAndShow doesn't decoded");
    }

}


void MainWindow::OnRefresh(unsigned char *p)
{
    _FrameImage->RefreshVideo(p);
}

void MainWindow::on_actionFull_triggered()
{
    _bPause = true;
    _FrameImage->DrawVideo(false);
    repaint();
    ui->actionFull->setChecked(true);
    ui->actionHalf->setChecked(false);
    QObject:disconnect(_decoder, SIGNAL(Refresh(unsigned char*)),this,0);
    MySleep::msleep(500);
    delete _decoder;
    _decoder = new CFFDecode();
    if (!_decoder->Init(_iCodec, VIDEO_W, VIDEO_H, _VIDEO_SCALE_FULL_W, _VIDEO_SCALE_FULL_H, 1000, 0, NULL))
    {   delete _decoder;
        _decoder = NULL;
        LogInfo("Decoder Init Failed");
        return;
    }
    QObject::connect(_decoder, SIGNAL(Refresh(unsigned char*)), this, SLOT(OnRefresh(unsigned char*)));
    _FrameImage->SetSize(_VIDEO_SCALE_FULL_W, _VIDEO_SCALE_FULL_H);
    _bPause = false;
}

void MainWindow::on_actionHalf_triggered()
{
    _bPause = true;
    _FrameImage->DrawVideo(false);
    repaint();
    ui->actionFull->setChecked(false);
    ui->actionHalf->setChecked(true);
    QObject:disconnect(_decoder, SIGNAL(Refresh(unsigned char*)),this,0);
    MySleep::msleep(500);
    delete _decoder;
    _decoder = new CFFDecode();
    if (!_decoder->Init(_iCodec, VIDEO_W, VIDEO_H, _VIDEO_SCALE_HALF_W, _VIDEO_SCALE_HALF_H, 1000, 0, NULL))
    {   delete _decoder;
        _decoder = NULL;
        LogInfo("Decoder Init Failed");
        return;
    }
    QObject::connect(_decoder, SIGNAL(Refresh(unsigned char*)), this, SLOT(OnRefresh(unsigned char*)));
    _FrameImage->SetSize(_VIDEO_SCALE_HALF_W, _VIDEO_SCALE_HALF_H);
    _bPause = false;
}

void MainWindow::on_actionLow_triggered()
{
    ui->actionLow->setChecked(true);
    ui->actionMid->setChecked(false);
    ui->actionHigh->setChecked(false);
    _myCamera->SetBitrate(80000);
}

void MainWindow::on_actionMid_triggered()
{
    ui->actionLow->setChecked(false);
    ui->actionMid->setChecked(true);
    ui->actionHigh->setChecked(false);
    _myCamera->SetBitrate(150000);
}

void MainWindow::on_actionHigh_triggered()
{
    ui->actionLow->setChecked(false);
    ui->actionMid->setChecked(false);
    ui->actionHigh->setChecked(true);
    _myCamera->SetBitrate(200000);
}

void MainWindow::on_actionFront_triggered()
{
    ui->actionFront->setChecked(true);
    ui->actionRear->setChecked(false);
    _myCamera->StopCamera();
    _myCamera->SetCamera(true);
    _myCamera->StartCamera();
}

void MainWindow::on_actionRear_triggered()
{
    ui->actionFront->setChecked(false);
    ui->actionRear->setChecked(true);
    _myCamera->StopCamera();
    _myCamera->SetCamera(false);
    _myCamera->StartCamera();
}

void MainWindow::on_actionEcho_triggered()
{
    ui->actionEcho->setChecked(true);
    ui->actionM2M->setChecked(false);
    _udp->InitSocket(9150,9151);
}

void MainWindow::on_actionM2M_triggered()
{
    ui->actionEcho->setChecked(false);
    ui->actionM2M->setChecked(true);
    _udp->InitSocket(9051,9050);
}
