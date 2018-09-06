#ifndef QSOCKETTHREAD_H
#define QSOCKETTHREAD_H

#include <QObject>
#include <QThread>
#include <QTcpSocket>

#include <common.h>


class QSocketThread : public QThread
{
public:
    QSocketThread(int descriptor, Param *param, string strn);
    ~QSocketThread();


    void run();
    Param **param;
    bool compression;
    bool encryption;
    string str;

public slots:
    void onReadyRead();
    void onDisconnected();

private:
    int socketDeskriptor;
    QTcpSocket *socket;
};

#endif // QSOCKETTHREAD_H
