#ifndef MYTHREAD_H
#define MYTHREAD_H
#include <QThread>
#include <QObject>
#include <QWidget>
#include "MyServerClass.h"

//https://habr.com/post/150274/

class MyThread : public QThread {
Q_OBJECT
public:
    MyThread(const QString& strHost, int nPort,  MyServer * pwgt = 0);
    ~MyThread();
     void run();
private slots:
    //void destroyThtread();

     //void closeThisThread();

signals:


//    {
//        exec();
//    }
};

#endif // MYTHREAD_H
