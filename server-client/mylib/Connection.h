#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QTcpSocket>
#include "Message.h"
#include "ConsoleHelper.h"


class MyTcpApp;

class Connection : public QObject
{
    Q_OBJECT
private:
    QTcpSocket *socket;
    MyTcpApp *clientThread;
    ConsoleHelper *consoleHelper;
    ConfigMessage configMessage;

public:
    Connection(QTcpSocket * = nullptr, QObject * = nullptr);
    virtual ~Connection();
    void send(Message &, bool info = false);
    void sendB(Message &, bool info);
    void send(MessageType);
    Message * receive(bool info = false);
    QTcpSocket * getRemoteSocket();
    void setClientThread(MyTcpApp *);
    void setConsoleHelper(ConsoleHelper *);
    void runLoop(bool);
    void setConfigMessage(ConfigMessage);

private slots:
    void disconnected();
    void mainLoop();
};


#endif // CONNECTION_H
