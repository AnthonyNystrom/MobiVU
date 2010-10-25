#ifndef THREADUDP_H
#define THREADUDP_H

#include <QThread>
#include <QUdpSocket>
#include <QTime>

class ThreadUDP : public QThread
{
    Q_OBJECT
public:
    ThreadUDP();
    ~ThreadUDP();
    void InitSocket(int iPort);
    void StopWrite();
    void Pause(bool bPause);
    int _iMax, _iMin, _iAvg, _iLost, _iNCampRx, _iNCampTx;
private slots:
    void readPendingDatagrams();
protected:
     void run();     
     void Write();     

     QUdpSocket *_udpSocket;
     int _iPort, _iProgr, _iProgrRead;
     unsigned int _iTot;
     QTime _Timer;
     bool _bDoWrite;
     bool _bPause;
};

#endif // THREADUDP_H
