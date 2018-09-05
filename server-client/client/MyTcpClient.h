#ifndef MYTCPCLIENT_H
#define MYTCPCLIENT_H

#include <QObject>
#include <QTcpSocket>

#include "../mylib/Connection.h"
#include "../mylib/MyTcpApp.h"

class MyTcpClient : public MyTcpApp
{
    Q_OBJECT
private:
    QString name;
    QString fullName;
    QString serverIP;
    quint16 serverPort;
    bool serverConnected;
    ConfigMessage configMessage;
    QTcpSocket *socket;
    Connection *connection;
    ConsoleHelper consoleHelper;
    QMap<QString, QString> config;

    bool handshake();
    void loop();

public:
    explicit MyTcpClient(QString = nullptr, QString = nullptr, QString = nullptr);
    virtual QString getFullName() const;
    virtual void clientDisconnect();
    virtual void clientLoop();
    void client_run();
};

#endif // MYTCPSERVER_H
