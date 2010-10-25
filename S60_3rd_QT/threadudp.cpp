#include "threadudp.h"
#include <QDebug>
#include <QMessageBox>
#include <in_sock.h>
#include "sym_iap_util.h"

ThreadUDP::ThreadUDP(MainWindow *iAppUi)
{
    _iAppUi = iAppUi;
    Init();
}

void ThreadUDP::Init()
{    
    _iCntVideo = 0;
    _udpSocketAudio=NULL;
    _udpSocketVideo=NULL;
    isDefaultIapSet = false;
}

ThreadUDP::~ThreadUDP()
{
    if (_udpSocketAudio)
        delete _udpSocketAudio;
    _udpSocketAudio=NULL;
    if (_udpSocketVideo)
        delete _udpSocketVideo;
    _udpSocketVideo = NULL;
}

void ThreadUDP::InitSocket(int iPortAudio, int iPortVideo)
{
    if (_udpSocketAudio){
        disconnect(_udpSocketAudio, SIGNAL(readyRead()), this, 0);
        disconnect(_udpSocketAudio, SIGNAL(error(QAbstractSocket::SocketError)), this, 0);
        delete _udpSocketAudio;
        _udpSocketAudio=NULL;
    }

    if (_udpSocketVideo){
        disconnect(_udpSocketVideo, SIGNAL(readyRead()), this, 0);
        disconnect(_udpSocketVideo, SIGNAL(error(QAbstractSocket::SocketError)), this, 0);
        delete _udpSocketVideo;
        _udpSocketVideo = NULL;
    }


    _byProgrAudio = _byProgrVideo = 0xFF;
    _bDoWrite = true;
    _iPortAudio = iPortAudio;
    _iPortVideo = iPortVideo;
    _udpSocketAudio = new QUdpSocket(this);
    _udpSocketVideo = new QUdpSocket(this);

    if(!isDefaultIapSet) {
        _iAppUi->LogInfo("qt_SetDefaultIap");
        qt_OfferIapDialog();
        isDefaultIapSet = true;
    }

    _udpSocketAudio->bind(QHostAddress::Any, _iPortAudio, QUdpSocket::DefaultForPlatform);    
    _udpSocketVideo->bind(QHostAddress::Any, _iPortVideo, QUdpSocket::DefaultForPlatform);
    connect(_udpSocketAudio, SIGNAL(readyRead()), this, SLOT(readPendingDatagramsAudio()));
    connect(_udpSocketVideo, SIGNAL(readyRead()), this, SLOT(readPendingDatagramsVideo()));
    connect(_udpSocketVideo, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayErrorVideo(QAbstractSocket::SocketError)));
    connect(_udpSocketAudio, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayErrorAudio(QAbstractSocket::SocketError)));    
}
void ThreadUDP::readPendingDatagramsAudio(){

    while (_udpSocketAudio->hasPendingDatagrams()) {
        QByteArray datagram;
        int iSizeData = _udpSocketAudio->pendingDatagramSize();        
        datagram.resize(_udpSocketAudio->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;
        _udpSocketAudio->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
        TBuf8<512> tmp;

        unsigned char *p = (unsigned char *)datagram.data();
        tmp.Copy(p+1, datagram.length()-1);
        unsigned char iCnt = *p;
        _iAppUi->_myCamera->_iAudioEngine->Play(tmp);
        //_iAppUi->LogInfo("ReadAudio:%d",iCnt);
    }
}

void ThreadUDP::readPendingDatagramsVideo()
{

    while (_udpSocketVideo->hasPendingDatagrams()) {
        QByteArray datagram;
        int iSizeData = _udpSocketVideo->pendingDatagramSize();
        datagram.resize(iSizeData);
        QHostAddress sender;
        quint16 senderPort;        

        _udpSocketVideo->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
        unsigned char *p = (unsigned char *)datagram.data();
        unsigned char iCnt = *p;
        p++;
        _iAppUi->DecodeAndShow(p, iSizeData-1);


        if (iCnt!=_iCntVideo+1){
            _iAppUi->LogInfo("video frames lost:%d", iCnt-_iCntVideo);
        }
        _iCntVideo = iCnt;
        //_iAppUi->LogInfo("ReadVideo:%d",iCnt);
    }
}
void ThreadUDP::WriteVideo(unsigned char *buff, int iSizeBuff)
{
    QHostAddress server("69.21.114.154");

    if (_udpSocketVideo!=NULL)
    {
        if (_byProgrVideo==0xFF)
            _byProgrVideo=0;
        else
            _byProgrVideo++;
        //if (_byProgrVideo%20){
            QByteArray datagram;
            datagram.append(_byProgrVideo);
            datagram.append((const char*)buff, iSizeBuff);

            _udpSocketVideo->writeDatagram(datagram.data(), datagram.length(), server, _iPortVideo);
        //_iAppUi->LogInfo("WriteVideo:%d",_byProgrVideo);
        //_udpSocketVideo->writeDatagram((char*)buff, iSizeBuff, server, _iPortVideo);
        //}
    }
}
void ThreadUDP::WriteAudio(unsigned char *buff, int iSizeBuff)
{

    QHostAddress server("69.21.114.154");
    if (_udpSocketAudio!=NULL)
    {
        if (_byProgrAudio==0xFF)
            _byProgrAudio=0;
        else
            _byProgrAudio++;
        QByteArray datagram;
        datagram.append(_byProgrAudio);
        datagram.append((const char*)buff, iSizeBuff);        
        _udpSocketAudio->writeDatagram(datagram.data(), datagram.length(), server, _iPortAudio);
    }
}

void ThreadUDP::displayErrorVideo(QAbstractSocket::SocketError socketError)
{
    ShowError(socketError, _udpSocketVideo);
}

void ThreadUDP::displayErrorAudio(QAbstractSocket::SocketError socketError)
{
    ShowError(socketError, _udpSocketAudio);
}

void ThreadUDP::ShowError(QAbstractSocket::SocketError socketError, QUdpSocket *socket)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        _iAppUi->LogInfo("ThreadUDP::ShowError->QAbstractSocket::RemoteHostClosedError");
        break;
    case QAbstractSocket::HostNotFoundError:
        _iAppUi->LogInfo("ThreadUDP::ShowError->QAbstractSocket::HostNotFoundError");
        QMessageBox::information(NULL, tr("MobiVu"),
                                 tr("The host was not found. Please check the "
                                    "host name and port settings."));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        _iAppUi->LogInfo("ThreadUDP::ShowError->QAbstractSocket::ConnectionRefusedError");
        QMessageBox::information(NULL, tr("MobiVu"),
                                 tr("The connection was refused by the peer. "
                                    "Make sure the fortune server is running, "
                                    "and check that the host name and port "
                                    "settings are correct."));
        break;
    default:
        _iAppUi->LogInfo("ThreadUDP::ShowError->%s", socket->errorString());
        QMessageBox::information(NULL, tr("MobiVu"),
                                 tr("The following error occurred: %1.")
                                 .arg(socket->errorString()));
    }
}

