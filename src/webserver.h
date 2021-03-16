#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <memory>
#include <map>

#include <QThread>
#include <QtNetwork/qtcpserver.h>
#include <QtNetwork/qtcpsocket.h>

using std::unique_ptr;

class TcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    TcpSocket();
    void run();
protected slots:
    void ReceiveDataSlot();
    void ClientDisconnectedSlot();
signals:
    void GetDataFromClient(QString, int);
    void ClientDisConnected(int);
private:
   int m_iDescriptor;
};


class WebServer : public QTcpServer
{
    Q_OBJECT
public:
    WebServer();
public:
    void StartWebServer();

    // QThread interface
public:
    void run();
private:

    // QTcpServer interface
public:
    //bool hasPendingConnections() const;
    //QTcpSocket *nextPendingConnection();

protected:
    void incomingConnection(qintptr handle);
private:
    QString ParseMessage(const QString &msg);
private slots:
    void SocketDataProcessing(QString sendData, int descriptor);
    void SocketDisconnected(int descriptor);
private:
    quint64 m_usUsePort;
    std::map<qintptr, QTcpSocket *> m_lstSockets;
};

#endif // WEBSERVER_H
