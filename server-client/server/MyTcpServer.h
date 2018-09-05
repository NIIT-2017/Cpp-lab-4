#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

#include "../mylib/Connection.h"
#include "../mylib/Message.h"
#include "ClientThread.h"

class MyTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    static ConsoleHelper *consoleHelper;
    static QList<QString> aphorisms;
    static int countAphorisms;
    static QFile *logFileSolution; // нужно только для условия задачи про логфайл
    static QTextStream *logSolution; // нужно только для условия задачи про логфайл
    static QMutex mutex; // нужно только для условия задачи про логфайл

    explicit MyTcpServer(QString = nullptr, QObject *parent = nullptr);
    ~MyTcpServer();
    void server_run();

public slots:
    void killClient(size_t);

protected:
    virtual void incomingConnection(qintptr socketDescriptor);

private:
    quint16 serverPort;
    size_t id = 0;
    int countUsers = 0;
    int maxUsers = 0;
    QMap<size_t, ClientThread *> usersMap;
    QMap<QString, QString> config;
    ConfigMessage configMessage;
   // bool isCompression;
    //bool isEncryption;
};

#endif // MYTCPSERVER_H
