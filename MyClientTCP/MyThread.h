#pragma once

#include <QtWidgets>

class MyWorker : public QObject {
Q_OBJECT
private:
    int    m_nValue;
    QTimer m_timer;

public:
    MyWorker(QObject* pobj = 0) : QObject(pobj)
                                , m_nValue(10)
    {
        connect(&m_timer, SIGNAL(timeout()), SLOT(setNextValue()));
    }

    void doWork()
    {
        m_timer.start(1000);
    }

signals:
    void valueChanged(int);
    void finished    (   );

public slots:
    void setNextValue()
    {
        emit valueChanged(--m_nValue);

        if(!m_nValue){
            emit finished();
        }
    }
};

// ======================================================================
class MyThread : public QThread {
Q_OBJECT
public:
    MyThread()
    {
    }

    void run()
    {
        exec();
    }
};
