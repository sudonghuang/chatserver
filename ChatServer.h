#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <QWidget>
#include<QTcpServer>
#include<QHostAddress>
QT_BEGIN_NAMESPACE
namespace Ui {
class QObject;
}
QT_END_NAMESPACE

class ChatServer : public QObject
{
    Q_OBJECT

public:
    explicit ChatServer(QObject *parent = nullptr);
    ~ChatServer();

    bool startServer(int port);//开启服务器
    void stopServer();//关闭服务器
    bool isRunning()const;
private slots:
    void newConnection();
    void clientDisconnected();
    void receiveMessage();
private:
    QTcpServer *tcpServer;
    QMap<QTcpSocket*,QString>clients;
    Ui::QObject *ui;

    void broadcastMessage(const QByteArray &message,QTcpSocket *exclude=nullptr);
};
#endif // CHATSERVER_H
