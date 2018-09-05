#ifndef MYTCPAPP_H
#define MYTCPAPP_H

#include <QString>
#include <QObject>

class MyTcpApp : public QObject
{
    Q_OBJECT
public:
    MyTcpApp(QObject * = nullptr);
    virtual QString getFullName() const = 0;
    virtual void clientDisconnect() = 0;
    virtual void clientLoop() = 0;
};

#endif // MYTCPAPP_H
