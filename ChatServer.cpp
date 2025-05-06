#include "ChatServer.h"
// #include "./ui_widget.h"
#include<QTcpSocket>
ChatServer::ChatServer(QObject  *parent)
    : QObject (parent),
    // , ui(new Ui::Widget),
    tcpServer(new QTcpServer(this))
{
    // ui->setupUi(this);
     connect(tcpServer, &QTcpServer::newConnection, this, &ChatServer::newConnection);

}

ChatServer::~ChatServer()
{
    // delete ui;
    stopServer();
}
bool ChatServer::startServer(int port){
    if(tcpServer->isListening()){
        stopServer();
    }
    return tcpServer->listen(QHostAddress::Any,port);
}
void ChatServer::stopServer(){
    if(tcpServer->isListening()){
        for(QTcpSocket* socket:clients.keys()){
            socket->disconnectFromHost();
        }
        clients.clear();
        tcpServer->close();
    }
}
bool ChatServer::isRunning()const//检测是否在运行
{
    return tcpServer->isListening();
}
void ChatServer::newConnection(){
    QTcpSocket*clientSocket=tcpServer->nextPendingConnection();//下一个追加的连接

    connect(clientSocket,&QTcpSocket::readyRead,this,&ChatServer::receiveMessage);
    connect(clientSocket,&QTcpSocket::disconnected,this,&ChatServer::clientDisconnected);

    clients.insert(clientSocket,"");//加入连接池S

    qDebug()<<"New client connect:"<<clientSocket->peerAddress().toString();//打印对方IP
}
void ChatServer::clientDisconnected(){
    QTcpSocket *socket=qobject_cast<QTcpSocket*>(sender());//检测是否是socket类型
    if(socket){
        qDebug()<<"Client disconnected:"<<socket->peerAddress().toString();
        if(!clients[socket].isEmpty()){
            QString userName=clients[socket];
            //通知其他客户端该用户离开
            QString msg="SYSTEM:"+userName+"离开了聊天室";
            broadcastMessage(msg.toUtf8());
        }
        clients.remove(socket);
        socket->deleteLater();
    }
}
void ChatServer::receiveMessage(){
    QTcpSocket *socket=qobject_cast<QTcpSocket*>(sender());
    if(!socket)return;

    QByteArray message=socket->readAll();
    if(message.isEmpty())return;

    QString strMsg=QString::fromUtf8(message);
    // qDebug()<<strMsg;//调试
    //解析消息格式用户名：消息
    int splitIndex=strMsg.indexOf(":");
    if(splitIndex>0){
        QString sender=strMsg.left(splitIndex);

        //如果是系统消息，处理加入/离开通知
        if(sender=="SYSTEM"){
            //存储用户名
            QString content=strMsg.mid(splitIndex+1);
            // qDebug()<<content;//调试
            if(content.contains("加入了聊天室")){
                QString userNAme=content.split(" ").first();
                clients[socket]=userNAme;
            }
            //广播给所有用户
            broadcastMessage(message);
        }else{
            //普通消息，
            if(clients[socket].isEmpty()){
                clients[socket]=sender;
            }
            //广播
            broadcastMessage(message);
        }

    }
}
void ChatServer::broadcastMessage(const QByteArray &message,QTcpSocket* exclude){
    for(QTcpSocket* socket:clients.keys()){
        if(socket !=exclude&&socket->state()==QTcpSocket::ConnectedState){
            socket->write(message);
        }
    }
}
