#include "mythread.h"
#include "MyServerClass.h"
#include "MySocket.h"
#include <QDebug>
MyThread :: MyThread (const QString &strHost, int nPort, MyServer *pwgt)
{
    //QString ThreadId = QString( "0x%1" ).arg( (int)this->currentThreadId(), 16 );
    //qDebug()<<"MyThread :: MyThread "<<ThreadId;
    MySocket * TcpSocket = new MySocket(strHost,nPort,pwgt);
    TcpSocket->moveToThread(this);
    connect(this, SIGNAL(finished()), this, SLOT(quit()));
}

MyThread::~MyThread()
{
    this->wait();
}

//void MyThread::destroyThtread()
//{
//    this->quit();
//}

void MyThread::run()
{//this->currentThreadId()
   // QString ThreadId = QString( "0x%1" ).arg( (int)this->currentThreadId(), 16 );
    //QString ThreadId = QString( "0x%1" ).arg( (int)QThread::currentThread(), 16 );
    //qDebug()<<"MyThread :: run() " <<ThreadId;
    exec();
}

//void MyThread::closeThisThread()
//{
//    this->quit();
//}
