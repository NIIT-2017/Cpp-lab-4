#ifndef CLIENTTHREAD_H
#define CLIENTTHREAD_H

#include <QTcpServer>
#include <QRunnable>

#include "../mylib/Connection.h"
#include "../mylib/ConsoleHelper.h"
#include "../mylib/MyTcpApp.h"

class MyTcpServer;

class ClientThread : public MyTcpApp, public QRunnable
{
    Q_OBJECT
private:
    bool clientConnected;
    ConfigMessage configMessage;
    Connection *connection;
    qintptr socketDescriptor;
    QString name;
    QString fullName;
    size_t id;

    bool handshake();

signals:
    void finished(size_t);

public:
    ClientThread(size_t = 0, qintptr = 0, ConfigMessage = ConfigMessage());
    virtual QString getFullName() const;
    virtual void clientDisconnect();
    virtual void clientLoop();

    virtual void run();
};

#endif // CLIENTTHREAD_H
