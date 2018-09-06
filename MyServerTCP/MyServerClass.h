
#pragma once

#include <QWidget>
#include <QTcpSocket>

class QTextEdit;
class QLineEdit;
class QListView;
class QListWidget;
class QSplitter;
class QLabel;
class QCheckBox;
//class QPair <int, QTcpSocket *> ;
//class QList <QPair>;
#include <QPair>
#include <QList>
#include <QAbstractSocket>

//Q_DECLARE_METATYPE(QAbstractSocket::SocketState)

class MyServer : public QWidget {
Q_OBJECT
private:
    QTcpSocket* m_pTcpSocket;
    QTextEdit*  m_ptxtInfo;//экран вывода сообщений
    QTextEdit*  WordInfo;//экран вывода сообщений
    QLineEdit*  m_ptxtInput;
    QLineEdit*  newPortnumber;
    quint16     m_nNextBlockSize;
    //QListView*  ListView;
    QSplitter *WidgetSplitter;
    QSplitter *Left2Splitter;
    QListWidget* ListWidget;
    QLabel *ServerLabel;
    QLabel *ListLabel ;
    QLabel *PortLabel ;
    //QList< QTcpSocket *> TcpSocket_List;//
    QList< int > Port_List;
    QCheckBox *ChekBxNeebZip;
    QCheckBox *ChekBxNeebCode;
    QStringList WordList;
    QStringList WordList2;
   // QPair <int, QTcpSocket *> Socket_Port; //парочки

    //QList < Socket_Port > Socket_Port_List;
   // QList < QPair <int, QTcpSocket *> > Socket_Port_List;
    void setupListWidget();
    void RemovePortFromList(int port);
    int randomBetween(int low, int high);
    void GetFile();

signals:
    void  SigbalDelPortFromBtm(int port);
    void  SigbalDelPortFromBtm2(int port);
    void  SigbalDelTread(int port);
    void  SignalSendToClient();
    void addPort ();
    void ChekBxNeebZipChange(bool);
    void ChekBxNeebCodeChange(bool);

public:
    MyServer(const QString& strHost, int nPort, QWidget* pwgt = 0) ;
    virtual ~MyServer();//http://forum.vingrad.ru/topic-360104.html
    QTextEdit* getTextEdit();//лучше в конструктор
    QLineEdit *getQLineEdit();
    bool getCheckStatus();
    bool getCheckCodeStatus();
private slots:
    //void slotReadyRead   (                            );
    //void slotError       (QAbstractSocket::SocketError);
    void slotSendToClient(                            );
    void slotConnected   (                            );
    void AddNewPort ();
    //void DeletePort ();
    void  clearTextField();
    void DelPort(int port);
    void test();
    void DelPortFromBtm();
    void TextPrint(QString);
    void GetChekBxNeebZip(bool);
    void GetChekBxNeeCode(bool);
    void setMaintext();
    void AddnewText();
};
