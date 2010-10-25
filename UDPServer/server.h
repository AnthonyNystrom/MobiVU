#ifndef SERVER_H
#define SERVER_H

// server.h
#include <QtNetwork>
#include <QObject>
#include <QThread>
#include <QUdpSocket>
#include <QVector>

#define MAX_USERS_CONNECTED 3

typedef struct {
  QHostAddress hostAddress;
  quint16 port;
  quint16 bindId;
} bindDesc;
typedef QList<bindDesc> bindTable;

typedef struct {
  QHostAddress hostAddress;
  quint16 port;
  quint16 bindId;
  unsigned char counter;
  int timeout;
} clientDesc;

//class Server: public QObject
class Server: public QThread
{
Q_OBJECT
public:
  Server(quint16 port, bool isEchoServer = false, QObject * parent = 0);
  ~Server();
  
  void resetCounters();
  void removeClientAtId(int index);
  void removeClient(clientDesc *utente);
  int howConnections();

  virtual void run();

  //void addHost(QHostAddress _hostAddress, quint16 _port, quint16 _bindId);// forse non serve

  bool addClientToList(QHostAddress clientIp, quint16 senderPort);
//  QHostAddress interlocutor(QHostAddress mittente);

  clientDesc *interlocutore(QHostAddress mittente, quint16 porta);
  clientDesc *mittente(QHostAddress mittente, quint16 porta);

  bool echoServer;

  // Contatori globali
  quint64 bytesReceived;
  quint64 packetsReceived;
  double kbps;
  quint64 lastBytesReceived;
  quint64 switchPortCounter;

  // statistiche
  QTime lastJittTime;
  double avgJitt;
  int minJitt;
  int maxJitt;

  // Lista degli utenti connessi al server
  clientDesc clientsList[MAX_USERS_CONNECTED];

  QHostAddress lastSenderAddress;
  quint16 lastSenderPort;

//  uint lastCounterReceived;
  unsigned char lastCounterReceived;

  uint pacchettiRicevuti;
  uint pacchettiPersi;
public slots:
  void readPendingDatagrams();
  void timerEvent(QTimerEvent *event);

private:
  quint16 serverPort;
  QUdpSocket* udpSocket;
//  bindTable bindTableList;

  void processTheDatagram(const QByteArray &datagram, const QHostAddress &address, quint16 port);
};


#endif // SERVER_H
