#include "ChatServer.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include<QCommandLineOption>
#include<QCommandLineParser>
#include<QStringList>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "ChatServer_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    QCommandLineParser parser;
    parser.setApplicationDescription("聊天服务器");
    parser.addHelpOption();

    QCommandLineOption portOption(QStringList()<<"p"<<"port","指定服务器监听端口（默认:9090)","port","9090");

    parser.addOption(portOption);

    parser.process(a);

    int port=parser.value(portOption).toInt();

    ChatServer Server;
    if(Server.startServer(port)){
        qDebug()<<"服务器已启动,监听端口："<<port;
    }else{
        qDebug()<<"服务器启动失败,端口可能被占用:"<<port;
        return a.exec();
    }
    ChatServer w;
    // w.show();
    return a.exec();
}
