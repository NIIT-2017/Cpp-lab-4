#include <QtWidgets>
#include <QtGui>
#include "MySocket.h"
#include "MyServerClass.h"
#include <QDebug>
#include <QWidget>
#include <QTcpSocket>
#include <QListView>
#include <QThread>
#include <QTime>
#include <QDateTime>
#include <zlib.h>
#include "mythread.h"
//https://habr.com/post/150274/
//https://mayaposch.wordpress.com/2011/11/01/how-to-really-truly-use-qthreads-the-full-explanation/
// ----------------------------------------------------------------------



MySocket::MySocket(const QString &strHost, int nPort, MyServer *pwgt)

{
    //this->m_ptxtInput->setText("6000");
    //AddNewPort() ;

    this->Socket = new QTcpSocket(this);
    this->Socket->connectToHost(strHost, nPort);
    //Socket->open(QIODevice::ReadWrite);
    this->TextEdit  = pwgt->getTextEdit();
    this->LineEdit = pwgt->getQLineEdit();
    this->port = nPort;
    this->strHost = strHost;
    this->needZip = pwgt->getCheckStatus();// из файла ?
    this->needCode = pwgt->getCheckCodeStatus();

    connect(Socket, SIGNAL(connected()), SLOT(slotConnected()));
    connect(Socket, SIGNAL(readyRead()), SLOT(slotReadyRead()));

    connect(Socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this,         SLOT(slotError(QAbstractSocket::SocketError))
            );

    connect(this, SIGNAL(removePort(int)),
            pwgt,         SLOT(DelPort(int))
            );

    connect(this, SIGNAL(TreadStop(int)),
            this,         SLOT(stopThread(int))
            );

    connect(this, SIGNAL(TextAdd(QString)),
            pwgt, SLOT(TextPrint(QString))
            );

    connect(pwgt, SIGNAL(SignalSendToClient()),
            this, SLOT(slotSendToClient())
            );


    connect(pwgt, SIGNAL(SigbalDelPortFromBtm2(int)),
            this, SLOT(stopThread(int))
            );

    connect(pwgt , SIGNAL(ChekBxNeebZipChange(bool)),
            this, SLOT(setneedZip(bool))
            );

    connect(pwgt , SIGNAL(ChekBxNeebCodeChange(bool)),
            this, SLOT(setneedCode(bool))
            );

    //Неудачные попытки создать поток и переместить сокет туда
    //    MyThread *thread  = new MyThread;
    //    this->moveToThread(thread);
    //    thread->start();

    //QString number = QString::number(port);

    //QString ThreadId = QString( "0x%1" ).arg( (int)QThread::currentThread(), 16 );
    //emit TextAdd("Working port # " + number + " tread # " + ThreadId);
    //TextEdit->append("Working port # " + number + " tread # " + ThreadId);



    //pwgt->getTextEdit();

}

// ----------------------------------------------------------------------
void MySocket::slotReadyRead()
{
    QTcpSocket * socket = qobject_cast<QTcpSocket*>(sender());
    QByteArray  arrBlock = socket->readAll();
    QByteArray ZiparrBlock;
    if (needZip)  ZiparrBlock = qUncompress(arrBlock);
    else ZiparrBlock = arrBlock;
    QDataStream out(&ZiparrBlock, QIODevice::ReadOnly);
    out.setVersion(QDataStream::Qt_5_3);
    out.device()->seek(0);
    quint16 size;
    QString str;

    out >>  m_nNextBlockSize;
    //qDebug()<<"m_nNextBlockSize= " << m_nNextBlockSize;
    //int Fase;
    //out   >> Fase;
    out   >> str; //--out
    out >> size;
    //setFasetoheckBox(Fase);
    QString massege = str;
    QString trumassege;
    if  (needCode)  trumassege = massege;
            else trumassege = massege;
            //encodeDecode("12", massege);?
    //if (str.isEmpty()) trumassege = "No right data";

    readServerLog(trumassege);

       //пока оставить
        emit TextAdd(trumassege);
        m_nNextBlockSize = 0;
}

// ----------------------------------------------------------------------
void MySocket::slotError(QAbstractSocket::SocketError err)
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    QString strError =
            "Error: " + (err == QAbstractSocket::HostNotFoundError ?
                             "The host was not found." :
                             err == QAbstractSocket::RemoteHostClosedError ?
                                 "The remote host is closed." :
                                 err == QAbstractSocket::ConnectionRefusedError ?
                                     "The connection was refused. Port #" + QString::number(port)
                                   :    QString(socket->errorString())
                                     );
    // strError.append("\n");
    strError.append("\nPort #" + QString::number(port) + " closed");
    emit TextAdd(strError);
    //TextEdit->append(strError);//вывод на экран

    emit removePort(port);
    //emit TreadStop(port);//вылет
}

// ----------------------------------------------------------------------
void MySocket::slotSendToClient()
{
    QByteArray  arrBlock;
    QString massege = LineEdit->text();
    QString trumassege;
    if  (needCode)  trumassege = encodeDecode("12", massege);
    else trumassege = massege;
   // qDebug()<<" trumassege " << trumassege;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_3);
    out.device()->seek(0);
    //++ out << quint16(0) << QTime::currentTime() << LineEdit->text();
    out <<  quint16(0);
    // out << LineEdit->text();//--out

    int Fase = getFasetoheckBox();
    //qDebug()<<"  Fase " <<  Fase;
    out << Fase;
    out << trumassege;
    out << quint16(arrBlock.size() - sizeof(quint16));
    //Zip++
    QByteArray ZiparrBlock;
    if (needZip)  ZiparrBlock=qCompress(arrBlock,9);
    else ZiparrBlock = arrBlock;
    //--

    Socket->write(ZiparrBlock);
    //qDebug()<<" MySocket::slotSendToClient() " << arrBlock;

    emit TextAdd("Port #" + QString::number(port) + " Send To Client " + "< " + massege + " >");


}

// ------------------------------------------------------------------
void MySocket::slotConnected()
{
    QString number = QString::number(port);
    QString ThreadId = QString( "0x%1" ).arg( (int)QThread::currentThread(), 16 );
    emit TextAdd("Relationship with port " + number + " success - from tread # " + ThreadId);

}

MySocket::~MySocket()
{
    Socket->close();
    QString number = QString::number(port);
    QString ThreadId = QString( "0x%1" ).arg( (int)QThread::currentThread(), 16 );

    //TextEdit->append("Working port # " + number + " closed tread # " + ThreadId + " destoyed");
    emit TreadStop(port);
    //qDebug()<<"currentThread()->wait()";
    QThread::currentThread()->wait();

}

void MySocket::stopThread(int port)
{
    if (this->port == port)
    {
        QString ThreadId = QString( "0x%1" ).arg( (int)QThread::currentThread(), 16 );
        emit TextAdd("QThread # " + ThreadId + " closed");
        QThread::currentThread()->quit();
    }
}

void MySocket::setneedZip(bool status)
{
    this->needZip = status;
   // qDebug()<<"this->needZip= " << this->needZip;
}

void MySocket::setneedCode(bool status)
{
    this->needCode = status;
    //qDebug()<<"this->needCode= " << this->needCode;
}



QString MySocket::encodeDecode(QString Stringkey, QString inputStr)
{
    QString result;
    int len=inputStr.length();
    char *input=(char *)malloc(len+1);
    QByteArray ba=inputStr.toLatin1();
    strcpy(input,ba.data());
    int inputLength = len;
    char *key=(char *)malloc(len);
    QByteArray ba2=Stringkey.toLatin1();
    strcpy(key,ba2.data());

    int keyLength = Stringkey.length();
    char output[inputLength];
    for (int i = 0; i < inputLength + 1; ++i)
    {
        output[i] = input[i] ^ key[i % keyLength + 1];
    }
    result= (QString::fromLatin1(output, inputLength));
    return result;
}

int MySocket::getFasetoheckBox()
{
    if (needZip==false && needCode==false) return 1;
    else if (needZip==true && needCode==false) return 2;
    else if (needZip==false && needCode==true) return 3;
    else if (needZip==false && needCode==false) return 4;

}

void MySocket::readServerLog(QString massege)
{
    //запись в фаил

}


