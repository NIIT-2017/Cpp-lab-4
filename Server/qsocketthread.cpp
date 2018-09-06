#include "qsocketthread.h"

QByteArray encodeStr(const QByteArray &str)
{
    quint32 key = 073; //шифр
    QByteArray arr=str;
    for(int i =0; i<arr.size(); i++)
        arr[i] = arr[i] ^ key;

    return arr;
}




QSocketThread::QSocketThread(int descriptor, Param *param, string strn) : socketDeskriptor (descriptor)
{
    compression=param->compression;
    encryption=param->encryption;
    str=strn;
}

QSocketThread::~QSocketThread()
{
    delete socket;
}


void QSocketThread::run()
{
    socket=new QTcpSocket();
    socket->setSocketDescriptor(socketDeskriptor);

    connect(socket,&QTcpSocket::readyRead,this,&QSocketThread::onReadyRead, Qt::DirectConnection);
    connect(socket,&QTcpSocket::disconnected,this,&QSocketThread::onDisconnected, Qt::DirectConnection);

    qDebug() << socketDeskriptor << "Client connected";

    if (str.size()>0)
    {

        QString qstr=QString::fromStdString(str);
        QByteArray buffer=qstr.toLocal8Bit();

        if (encryption==true)buffer=encodeStr(buffer);
        if (compression==true)buffer=qCompress(buffer);


        socket->write(buffer);
    }



    exec();
}

void QSocketThread::onReadyRead()
{
    QByteArray Data=socket->readAll();
    QString qst(Data);
    string result=qst.toStdString(), itog;
    itog=result+"    "+str;
    if (result.size()>0)
    {

     QFile file("Logs.txt");
          if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
             file.write((char*)itog.c_str());
          }
          file.close();
    }
   socket->disconnectFromHost();
}

void QSocketThread::onDisconnected()
{
    socket->close();
    quit();
}






