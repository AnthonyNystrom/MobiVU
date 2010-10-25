#ifndef THREADUDP_H
#define THREADUDP_H

#include <QThread>
#include <QUdpSocket>
#include <QTime>
#include "mainwindow.h"

class MainWindow;

class ThreadUDP : public QObject
{
    Q_OBJECT
public:
    ThreadUDP(MainWindow *iAppUi);    
    ~ThreadUDP();
    void Init();
    void InitSocket(int iPortAudio, int iPortVideo);
    void WriteVideo(unsigned char *buff, int iSizeBuff);
    void WriteAudio(unsigned char *buff, int iSizeBuff);
private slots:
    void readPendingDatagramsAudio();
    void readPendingDatagramsVideo();
    void displayErrorAudio(QAbstractSocket::SocketError socketError);
    void displayErrorVideo(QAbstractSocket::SocketError socketError);
protected:     
     void ShowError(QAbstractSocket::SocketError socketError, QUdpSocket *socket);

     QUdpSocket *_udpSocketAudio, *_udpSocketVideo;
     int _iPortAudio, _iPortVideo;
     unsigned char _iCntVideo;
     bool _bDoWrite;
     bool _bPause;
     bool isDefaultIapSet;
     MainWindow *_iAppUi;
     unsigned char _byProgrVideo, _byProgrAudio;
};

#endif // THREADUDP_H
