#ifndef CLIENT_H
#define CLIENT_H

#include <QTcpSocket>
#include <QDebug>
#include <string>
#include <cstring>
#include "QTime"
#include <QFile>
#include "iostream"

using namespace std;

struct Param
{
    string adress;
    int  port;
    bool decompression;
    bool decoding;
};

class Client:public QTcpSocket
{
    Q_OBJECT
public:
    explicit Client(char *name);

    Param param;

    QTcpSocket *socket;

    bool read1(string nstr);
    int fill(char *a, char *b, int k);

public slots:
    void sockReady();
    void sockDisc();
};

#endif // CLIENT_H
