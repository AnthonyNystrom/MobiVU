// server.cc
#include "server.h"
#include <QHostAddress>

#define TIMEOUT_VALUE 2	// 2 sec.

#define DELICATO
#undef BRUTALE

#ifdef DELICATO
    #undef BRUTALE
#endif

//
// Costruttore della classe
//
Server::Server(quint16 _port, bool isEchoServer, QObject* parent): QThread(parent)
{
    serverPort = _port;

    echoServer=isEchoServer;

    lastCounterReceived=0;
    pacchettiRicevuti=0;
    pacchettiPersi=0;

    // Inizializzo le mie variabili
    resetCounters();

    // Inizializzo la lista dei clienti connessi
    for (int i=0; i<MAX_USERS_CONNECTED; i++) {
	// Disconnetto tutti gli utenti dal server
	removeClientAtId(i);
    }
}


//
// Cancello l'utente che Ë connesso al server 
//    
void Server::removeClient(clientDesc *utente)
{
	// L'utente non Ë valido quindi non faccio niente.
	if (utente == NULL) return;

	// Cancello i dati del cliente connesso	
        utente->hostAddress=QHostAddress::Null;
        utente->port=0;
        utente->counter=0;
        utente->timeout=0;
        utente->bindId=0;
}


//
// Cancello l'utente che Ë connesso al server e che si trova all'indice "index"
//    
void Server::removeClientAtId(int index)
{
	// Verifico l'indice ricevuto in ingresso
	if (index < 0) return;
	if (index >= MAX_USERS_CONNECTED) return;

	// Cancello i dati del cliente presente in quella posizione
        clientsList[index].hostAddress=QHostAddress::Null;
        clientsList[index].port=0;
        clientsList[index].counter=0;
        clientsList[index].timeout=0;
        clientsList[index].bindId=0;
}


//
// Azzeramento dei contatori
//
void Server::resetCounters()
{
    lastBytesReceived=0;
    bytesReceived=0;
    packetsReceived=0;
    kbps=0.0;
    
    avgJitt=0.0;
    minJitt=99999;
    maxJitt=0;

    pacchettiRicevuti=0;
    pacchettiPersi=0;
    switchPortCounter=0;
}


//
// Riporta il numero di connessioni attive sul server
//
int Server::howConnections()
{
    int quanteConnessioni = 0;
    
    for (int i=0;i<MAX_USERS_CONNECTED;i++) {
        // E' gi√  nella lista - Devo matchare sia l'IP che la porta in quanto potrei avere pi˘ di un utente connesso attraverso lo stesso IP
        if ( clientsList[i].hostAddress != QHostAddress::Null ) quanteConnessioni++;
    }
    
    return quanteConnessioni;
}


Server::~Server()
{
}


//
// Slot aggangiato alla ricezione di un pacchetto UDP
//
void Server::readPendingDatagrams()
{
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        qint64 rxLen = udpSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
        //qDebug()<< " Ricevuti " << rxLen << " bytes";
	
	// Se ho dei dati nel pacchetto ricevuto li "servo"
        if (rxLen>0) {
            lastSenderAddress = sender;
            lastSenderPort = senderPort;//udpSocket->localPort();

	    // Gestisco la tabella dei clienti
            addClientToList(sender, senderPort);

            // Analizzo il pacchetto richevuto
            processTheDatagram(datagram, sender, senderPort);
        } else qDebug("RXlen==0");
    }
}

//
// Aggiungo il client alla mia lista nel caso non sia gi√  presente
// Riporta true nel caso il cliente sia nuovo
//
bool Server::addClientToList(QHostAddress clientIp, quint16 senderPort)
{
    // Per prima cosa guardo se sono gi√  presente nella lista
    for (int i=0;i<MAX_USERS_CONNECTED;i++) {
        // E' gi√  nella lista - Devo matchare sia l'IP che la porta in quanto potrei avere pi˘ di un utente connesso attraverso lo stesso IP
        if (( clientsList[i].hostAddress == clientIp ) && (clientsList[i].port == senderPort) ) {
//        if ( clientsList[i].hostAddress == clientIp ) {
            clientsList[i].port = senderPort;
            clientsList[i].timeout = TIMEOUT_VALUE;
            
            // E' gi‡ nella lista quindi non vado neppure oltre
            return false;
        }
    }

#ifdef BRUTALE
    // Metodo BRUTALE:
    // Scenario: Devo aggiungere un utente ma su questa porta ne ho gi‡ due connessi
    // Operazione: Cancello tutti gli utenti loggati e riconnetto l'utente appena arrivato
    if ( howConnections() >= 2 ) {
	qDebug() << "SIAMO IN TROPPI: Resetto!!!";
        for (int i=0;i<MAX_USERS_CONNECTED;i++) {
		// Disconnetto tutti gli utenti dal server
		removeClientAtId(i);
        }
    }
#endif

    // Non sono nella lista quindi cerco un buco
    for (int i=0;i<MAX_USERS_CONNECTED;i++) {
        if ( clientsList[i].hostAddress.isNull() )  {
            clientsList[i].hostAddress = clientIp;
            clientsList[i].port = senderPort;
            clientsList[i].timeout = TIMEOUT_VALUE;
            qDebug() << QTime::currentTime().toString() << "Aggiunto " << clientIp.toString() << ":" << senderPort;

            return true;
        }
    }
    
    qDebug() << "NON HO TROVATO SPAZIO AGGIUNGI:: NON LO VOGLIO + VEDERE";
    
    // Non ho trovato spazio per aggiungere il nuovo cliente
    return false;
}


//
// Riporta l'indirizzo del destinatario in base al mittente
// Nel caso il server lavori in echo riporta il puntatore al mittente
//
clientDesc *Server::interlocutore(QHostAddress mittente, quint16 porta)
{
    // Ricavo l'indirizzo di destinazione del pacchetto che dovrÚ spedire
    for (int i=0; i<MAX_USERS_CONNECTED; i++) {
	// Analizzo solo i clienti connessi quelli nulli/vuoti li ignoro
        if (clientsList[i].hostAddress == QHostAddress::Null) continue;

        if (echoServer) {
	    // Mittente e clientsList[i] matchano
            if (( mittente == clientsList[i].hostAddress ) && (porta == clientsList[i].port)) return &clientsList[i];
        } else {
    	    // Sto analizzando il mittente quindi visto che NON sono in echo lo bypasso
    	    if (( mittente == clientsList[i].hostAddress ) && (porta == clientsList[i].port)) continue;

    	    // Riporto le info del destinatario
    	    return &clientsList[i];
        }
     }

    //qDebug() << "Manca l'interlocutore"  << mittente.toString() << ":" << porta;
    return NULL;
}


//
// Riporta l'indirizzo (nell'array dei clienti connessi) del mittente
//
clientDesc *Server::mittente(QHostAddress mittente, quint16 porta)
{
    for (int i=0; i<MAX_USERS_CONNECTED; i++) {
            if ( mittente == clientsList[i].hostAddress ) return &clientsList[i];
    }
     
    return NULL;
}

//
// Analizzo il pacchetto ricevuto
//
void Server::processTheDatagram(const QByteArray &datagram, const QHostAddress &address, quint16 port)
{
    //char comando;
    //comando = datagram[0];

    // Ricavo i dati del destinatario
    clientDesc *destinatario = interlocutore(address, port);

    // Ricavo i dati del mittente
    clientDesc *_mittente = mittente(address, port);

    // Aggiorno i contatori
    bytesReceived+=datagram.count();
    packetsReceived++;

    const char *buffer=datagram.constData();
    unsigned char progressivo = *((char*)&buffer[0]);

    //qDebug("ricevuto progressivo %d %d", lastCounterReceived, progressivo);
    // Verifico il progressivo
    if ( (progressivo) && ( _mittente != NULL) ) {
        // Tempi medi tra i pacchetti
        QTime now = QTime::currentTime();
        int deltaFromLast = lastJittTime.msecsTo(now);
        lastJittTime = now;

        // Calcolo i tempi
        avgJitt = ((avgJitt * (packetsReceived-1)) + deltaFromLast) / packetsReceived;
        if (minJitt>deltaFromLast) minJitt=deltaFromLast;
        else if (maxJitt<deltaFromLast) maxJitt=deltaFromLast;

        if (progressivo != ((_mittente->counter+1) & 0xFF)) {
            if (progressivo < _mittente->counter) qDebug("CASINO %d %d!!!!!", progressivo, _mittente->counter);
            pacchettiPersi += ((progressivo - _mittente->counter - 1) & 0xFF);
 //       } else  {
 //           qDebug("OK %d %d!!!!!", progressivo, _mittente->counter);
        }
    }
    _mittente->counter=progressivo;
    lastCounterReceived=progressivo;

    pacchettiRicevuti++;

    // Di default non devo rimuovere destinatari. Li elimino solo nel caso in cui ci siano pi˘ di due utenti 
    // connessi contemporaneamente sulla stessa porta del server.
    bool rimuoviDestinatari = false;
    
    // Metto un watchdog sul loop
    int watchDog = 0;
    
    // Inoltro il pacchetto
    while ( destinatario != NULL ) {
	// L'hostAddress ha un indirizzo farlocco quindi abbandono tutto
	if ( destinatario->hostAddress == QHostAddress::Null ) break;
	
	// Mando il datagramma al destinatario
	//qDebug() << " Inviati da " << address.toString() << ":" <<port << " a " << destinatario->hostAddress.toString()<<":"<<destinatario->port;
    	udpSocket->writeDatagram(datagram, destinatario->hostAddress, destinatario->port);
    		
#ifdef DELICATO
	if ( ! echoServer ) {
    	    // Metodo 2 - DELICATO: Se ho pi˘ di due clienti connessi li ciclo tutti (e gli mando il datagramma)
    	    //                      dopo di che li rimuovo dalla lista dei clienti. Nella lista alla fine dovrei avere solo il mittente.
    	    if ( ( howConnections() > 2 ) || ( rimuoviDestinatari ) ) {
                if ( ! rimuoviDestinatari ) {
                    switchPortCounter++;
                }

    		// Memorizzo il fatto che devo cancellare tutti i destinatari ancora connessi (eccetto il mittente)
    		rimuoviDestinatari = true;
    	    
    		// Rimuovo il destinatario a cui ho appena mandato il datagramma
    		removeClient( destinatario );

    		// Vado a cercare l'altro (escluso il mittente)
		destinatario = interlocutore(address, port);
    	    }
	}
	
	// Se non devo rimuovere destinatari non devo neanche stare a looppare qui dentro
    	if ( ! rimuoviDestinatari ) break;
	
	// Sicurezza anti bombaggio
	watchDog++;
	if ( watchDog > MAX_USERS_CONNECTED ) break;
#else
	break;
#endif
    } // end while
}


//
// Timer chiamato ogni secondo per la gestione delle statistiche generali
//
void Server::timerEvent(QTimerEvent *event)
{
    // Statistiche
    kbps = (bytesReceived-lastBytesReceived)/1000.0;
    lastBytesReceived = bytesReceived;

    // Gestione del timeout della connessione degli utenti
    for (int i=0; i<MAX_USERS_CONNECTED; i++) {
        if (clientsList[i].timeout) {
            clientsList[i].timeout--;

            if (!clientsList[i].timeout) {
                qDebug() << QTime::currentTime().toString() << "Disconnesso " << clientsList[i].hostAddress.toString();

		// Rimuovo il cliente dalla lista degli utenti connessi
		removeClientAtId(i);
            }
        }
    }
}


//
// Mando in run il server UDP
//
void Server::run()
{
    // Inizializzo le mie variabili
    resetCounters();

    // Inizializzo la lista dei clienti connessi
    for (int i=0; i<MAX_USERS_CONNECTED; i++) {
	// Disconnetto tutti gli utenti dal server
	removeClientAtId(i);
    }

    // Inizializzo il socket
    udpSocket = new QUdpSocket(this);
    
//    if (! udpSocket->bind(QHostAddress::Any, serverPort, QUdpSocket::ShareAddress|QUdpSocket::ReuseAddressHint)) qDebug("Errore on port %d", serverPort);
    if (! udpSocket->bind(QHostAddress::Any, serverPort, QUdpSocket::DefaultForPlatform)) qDebug("Errore on port %d", serverPort);

    // Aggancio il segnale della ricezione dei dati
    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(readPendingDatagrams()));

    // Faccio partire un timer per la gestione dei timeout e delle statistiche
    startTimer(1000);
}
