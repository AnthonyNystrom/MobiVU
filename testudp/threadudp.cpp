#include "threadudp.h"
#include <QDebug>

unsigned char buff[1100];

ThreadUDP::ThreadUDP()
{
    buff[0]=14;
    _udpSocket=NULL;    
}
ThreadUDP::~ThreadUDP()
{
    if (_udpSocket)
        delete _udpSocket;
    _udpSocket=NULL;
}
void ThreadUDP::run()
{
    _Timer.start();
    _iTot = _iNCampRx = _iMax = _iAvg = _iLost = _iProgr = _iProgrRead = _iNCampTx = _iNCampRx = 0;
    _iMin = 100000;
    _bPause = false;
    while(_bDoWrite)
    {
        if (!_bPause){
            Write();
            msleep(66);
        }
        else
            msleep(200);
    }
}
void ThreadUDP::InitSocket(int iPort)
{    
    _bDoWrite = true;
    _iPort = iPort;
    _udpSocket = new QUdpSocket(this);
    _udpSocket->bind(QHostAddress::Any, _iPort, QUdpSocket::DefaultForPlatform);
    connect(_udpSocket, SIGNAL(readyRead()), this, SLOT(readPendingDatagrams()));
    start();
}
void ThreadUDP::readPendingDatagrams()
{
    while (_udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        int iSizeData = _udpSocket->pendingDatagramSize();       
        datagram.resize(_udpSocket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;
        _udpSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
        unsigned int iProgr=0, iTS=0;
        unsigned char *p = (unsigned char *)datagram.data();
        iProgr |= p[4];
        iProgr<<=8;
        iProgr |= p[3];
        iProgr<<=8;
        iProgr |= p[2];
        iProgr<<=8;
        iProgr |= p[1];

        if (iProgr!=_iProgrRead){
            _iLost+=abs((iProgr-_iProgrRead));
            qDebug("LOSTED:%d", _iLost);
         }
        //qDebug("Counter:%d-%d", iProgr, _iNCampRx);
        _iProgrRead=iProgr+1;

        iTS |= (unsigned int)p[8];
        iTS<<=8;
        iTS |= (unsigned int)p[7];
        iTS<<=8;
        iTS |= (unsigned int)p[6];
        iTS<<=8;
        iTS |= (unsigned int)p[5];

        int iT1 = _Timer.elapsed();
        int iT = iT1-iTS;
        if (iT>_iMax)
            _iMax = iT;
        if (iT<_iMin)
            _iMin = iT;
        _iNCampRx++;
        _iTot += iT;
        if (_iNCampRx)
            _iAvg = _iTot / _iNCampRx;

        //qDebug("readPendingDatagrams:%d %d %d",datagram.size(), iProgr, iTS);

    }
}
void ThreadUDP::Write()
{    
    QHostAddress server("69.21.114.154");
    if (_udpSocket!=NULL)
    {
        buff[1] = _iProgr & 0x000000FF;
        buff[2] = (_iProgr >> 8 )& 0x000000FF;
        buff[3] = (_iProgr >> 16 )& 0x000000FF;
        buff[4] = (_iProgr >> 24 )& 0x000000FF;
        _iProgr++;
        unsigned int nMilliseconds = _Timer.elapsed();
        buff[5] = nMilliseconds & 0x000000FF;
        buff[6] = (nMilliseconds >> 8 )& 0x000000FF;
        buff[7] = (nMilliseconds >> 16 )& 0x000000FF;
        buff[8] = (nMilliseconds >> 24 )& 0x000000FF;

        _udpSocket->writeDatagram((char*)buff, sizeof(buff), server, _iPort);
        //qDebug("Write:%d %d", _iProgr, nMilliseconds);
        _iNCampTx++;
    }
}

void ThreadUDP::StopWrite()
{
    _bDoWrite=false;    
    //qDebug("Samples:%d Max:%d Min:%d Avg:%d Lost:%d", _iNCampRx, _iMax, _iMin, _iAvg, _iLost);
}

void ThreadUDP::Pause(bool bPause){
    _bPause=bPause;
}
