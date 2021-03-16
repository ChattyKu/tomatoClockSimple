#include "webserver.h"

#include <QMap>
#include <QDateTime>
#include <QFile>

const quint64 g_usBestPort = 80;
const quint64 g_usBeginNormalPort = 8080;

const QMap<qint64, QString> g_mapStatusCode{{200, "200 OK"}, {404, "404 NOT FOUND"}};

TcpSocket::TcpSocket()
{}

void TcpSocket::run()
{
    m_iDescriptor = this->socketDescriptor();
    connect(this, SIGNAL(readyRead()), this, SLOT(ReceiveDataSlot()));
    connect(this, SIGNAL(disconnect()), this, SLOT(ClientDisconnectedSlot()));
}

void TcpSocket::ReceiveDataSlot()
{
    QByteArray buffer;
    buffer = readAll();
    if (!buffer.isEmpty()) {
        QString data = QString::fromLocal8Bit(buffer);
        emit GetDataFromClient(data, m_iDescriptor);
    }
}

void TcpSocket::ClientDisconnectedSlot()
{
    emit ClientDisConnected(m_iDescriptor);
}



WebServer::WebServer()
{}

void WebServer::StartWebServer()
{}

void WebServer::run()
{
    if (this->isListening()) {
        return;
    }

    bool listend = this->listen(QHostAddress::LocalHost, g_usBestPort);
    if (listend) {
        m_usUsePort = g_usBestPort;
        return;
    }

    quint64 portNum = g_usBeginNormalPort;
    do {
        if (portNum == 0xFF) {
            qDebug() << "listen port is failed!";
            m_usUsePort = 0;
            return;
        }
        listend = this->listen(QHostAddress::LocalHost, portNum++);
    } while (!listend);
    m_usUsePort = portNum - 1;
}

void WebServer::incomingConnection(qintptr handle)
{
    TcpSocket *tcpsock = new TcpSocket();
    tcpsock->setSocketDescriptor(handle);
    tcpsock->run();
    connect(tcpsock, SIGNAL(GetDataFromClient(QString, int)), this, SLOT(SocketDataProcessing(QString, int)));
    connect(tcpsock, SIGNAL(ClientDisConnected(int)), this, SLOT(SocketDisconnected(int)));
    m_lstSockets[handle] = tcpsock;
}

QString WebServer::ParseMessage(const QString &msg)
{
    QString res;
    QString tmp = msg.left(msg.indexOf(' '));
    if (tmp.compare("GET") == 0) {
        QFile file("hello.html");
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            return "";
        }
        ///
        struct tm newtime;
        char szDT[50] = {0};
        time_t ltime;
        time(&ltime);
        gmtime_s(&newtime, &ltime);
        strftime(szDT, 128, "%a, %d %b %Y %H:%M:%S GMT", &newtime);
        ///
        auto fileArr = file.readAll();
        qDebug() << "msg:" << msg;
        res = QString("HTTP/1.0 %1\r\nDate: %2\r\nServer: %3\r\nAccept-Ranges: bytes\r\nContent-Length: %4\r\nConnection: %5\r\nContent-Type: %6\r\n\r\n")
                .arg(g_mapStatusCode[200])
                .arg(szDT)
                //.arg(QDateTime::currentDateTime().toUTC().toString("ddd, d MMM yyyy hh:mm:ss GMT"))
                .arg("tomato clock").arg(fileArr.size()).arg("Keep-Alive").arg("text/html");
        res.append(fileArr);
        return res;
    } else if (tmp.compare("PUT") == 0) {

        return "";
    } else {
        return "";
    }
    return "";
}

void WebServer::SocketDataProcessing(QString sendData, int descriptor)
{
    auto tmp = m_lstSockets.find(descriptor);
    if (tmp == m_lstSockets.end()) {
        return;
    }
    QTcpSocket *item = tmp->second;
    if (item->socketDescriptor() == descriptor) {
        qDebug() << "From --->" << item->peerAddress().toString() << ":" << item->peerPort();
        qDebug() << "[RecvData]:" << sendData;
        qDebug() << "End  <---";
        QString res = ParseMessage(sendData);
        item->write(res.toUtf8());
    }
}



void WebServer::SocketDisconnected(int descriptor)
{
    auto tmp = m_lstSockets.find(descriptor);
    if (tmp == m_lstSockets.end()) {
        return;
    }
    m_lstSockets.erase(tmp);
}

