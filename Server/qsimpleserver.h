#ifndef QSIMPLESERVER_H
#define QSIMPLESERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <qsocketthread.h>
#include <common.h>


class QSimpleServer : public QTcpServer
{
  Q_OBJECT
  Param param;


public:
    QTcpSocket *socket;
    vector <string> vstr;

    QSimpleServer();
    void incomingConnection(qintptr handle);
    bool read1();
    bool read2 ();
    int fill(char *a, char *b, int k);


};

#endif // QSIMPLESERVER_H
