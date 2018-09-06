#pragma once

#include <QWidget>

class QTcpServer;
class QTextEdit;
class QTcpSocket;
class QCheckBox;
// ======================================================================
class MyClient : public QWidget {
Q_OBJECT
private:
    QTcpServer* m_ptcpServer;//замена на QTcpSocket вероятна , но нужно отдавать обратно данные -> пока QTcpServer
    QTextEdit*  m_ptxt;
    quint16     m_nNextBlockSize;
    int nPort;
    bool needZip;
    bool needCode;
    qintptr socketDescriptor;
    QCheckBox *ChekBxNeebZip;
    QCheckBox *ChekBxNeebCode;


    void sendToServer(QTcpSocket* pSocket, const QString& str);
    QString encodeDecode(QString Stringkey, QString inputStr);
    int setFasetoheckBox(int);

public:
    MyClient(const QString & strHost,int nPort, QWidget* pwgt = 0);

public slots:
    virtual void slotNewConnection();
            void slotReadServer   ();
            void clearTextField();
            void SetChekBxNeebZip(bool);
            void SetChekBxNeebCode(bool);
};

