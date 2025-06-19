#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

#include <QtNetwork>
#include <QByteArray>
#include <QDebug>
#include <QList>

class MyTcpServer : public QObject
{
    Q_OBJECT
public:
    explicit MyTcpServer(QObject *parent = nullptr);
    ~MyTcpServer();
public slots:
    void slotNewConnection();
    void slotClientDisconnected(QTcpSocket* socket); // изменено
    void slotServerRead(QTcpSocket* socket);         // изменено
private:
    QTcpServer * mTcpServer;
    QList<QTcpSocket*> clients;                      // добавили список клиентов
};

#endif // MYTCPSERVER_H
