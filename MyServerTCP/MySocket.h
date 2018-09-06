
#pragma once

#include <QWidget>
#include <QTcpSocket>
class MyServer;
class QTextEdit;
class QLineEdit;
class QListView;
//#include <QTextCursor>
//Q_DECLARE_METATYPE (QTextCursor)
// ======================================================================
class MySocket : public QTcpSocket {
Q_OBJECT
private:
    //QTcpSocket* m_pTcpSocket;

    QLineEdit*  m_ptxtInput;
    QLineEdit*  newPortnumber;
    quint16     m_nNextBlockSize;
    bool needZip;
    bool needCode;
   // QList< QTcpSocket *> TcpSocket_List;//
    //QList< int > Port_List;
    QTcpSocket * Socket;
    int port;
    QString strHost;
    //void DeletePort ();
    QString encodeDecode(QString Stringkey, QString inputStr);
    int getFasetoheckBox();
    void readServerLog(QString massege);

public:
   MySocket(const QString& strHost, int nPort,  MyServer * pwgt = 0) ;
    virtual ~MySocket ();//http://forum.vingrad.ru/topic-360104.html
   QTextEdit*  TextEdit;
   QLineEdit*  LineEdit;
   //?

signals:
void removePort(int port);
void TreadStop(int port);
void TextAdd(QString);
//void MainTextAdd(QString);


private slots:

    void slotReadyRead   (                            );
    void slotError       (QAbstractSocket::SocketError);
    void slotSendToClient(                            );
    void slotConnected   (                            );
    void stopThread(int port);
    void setneedZip(bool);
    void setneedCode(bool);

};
