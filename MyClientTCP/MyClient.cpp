#include "MyClient.h"
#include <QDebug>
#include <QCoreApplication>
#include <QtNetwork>
#include <QtWidgets>
#include <QTextCodec>
#include <QDataStream>
#include <QDateTime>
#include <QtWidgets>
#include <QtGui>
MyClient::MyClient(const QString & strHost, int nPort, QWidget* pwgt /*=0*/) : QWidget(pwgt)
  , m_nNextBlockSize(0)
{


    this->needZip= true;
    this->needCode= true;
    this->nPort = nPort;
    m_ptcpServer = new QTcpServer(this);
    QTcpSocket* pClientSocket=new QTcpSocket;

    if (!m_ptcpServer->listen(QHostAddress::Any, nPort)) {
        //if (!m_ptcpServer->listen(QHostAddress Class, nPort)) {
        QMessageBox::critical(0,
                              "Client Error",
                              "Unable to start the server:"
                              + m_ptcpServer->errorString()
                              );
        m_ptcpServer->close();
        return;
    }
    connect(m_ptcpServer, SIGNAL(newConnection()),
            this,         SLOT(slotNewConnection())
            );

    m_ptxt = new QTextEdit;
    m_ptxt->setReadOnly(true);

    QPushButton* ClearBtm = new QPushButton("&Clear");
    connect(ClearBtm, SIGNAL(clicked()), SLOT(clearTextField()));

    ChekBxNeebZip = new QCheckBox("Файл сжат");
    ChekBxNeebZip->setChecked(true);

    connect(ChekBxNeebZip , SIGNAL(clicked(bool)),
            this, SLOT(SetChekBxNeebZip(bool))
            );

    ChekBxNeebCode = new QCheckBox("Файл закодирован");
    ChekBxNeebCode->setChecked(true);

    connect(ChekBxNeebCode , SIGNAL(clicked(bool)),
            this, SLOT(SetChekBxNeebCode(bool))
            );
    //Layout setup
    QVBoxLayout* pvbxLayout = new QVBoxLayout;
    pvbxLayout->addWidget(new QLabel("<H1>Client</H1>"));

    pvbxLayout->addWidget(m_ptxt);
    pvbxLayout->addWidget(ClearBtm);

    QVBoxLayout* HLayout = new QVBoxLayout;
    HLayout->addWidget(ChekBxNeebZip);
    HLayout->addWidget(ChekBxNeebCode);
    pvbxLayout->addLayout(HLayout);
    setLayout(pvbxLayout);


}

 void MyClient::slotNewConnection()
{
    QTcpSocket* pClientSocket = m_ptcpServer->nextPendingConnection();

    connect(pClientSocket, SIGNAL(disconnected()),
            pClientSocket, SLOT(deleteLater())
            );
    connect(pClientSocket, SIGNAL(readyRead()),
            this,          SLOT(slotReadServer())
            );

    sendToServer(pClientSocket, "Client Response: Connected!");
}

// ----------------------------------------------------------------------
void MyClient::slotReadServer()
{
    //http://doc.qt.io/qt-5/qtextcodec.html
    QTcpSocket* pClientSocket = (QTcpSocket*)sender();
    QByteArray  arrBlock = pClientSocket->readAll();
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
    int Fase;
    out   >>Fase;
    out   >> str; //--out
    out >> size;
    //setFasetoheckBox(Fase);
    QString massege = str;
    QString trumassege;
    if  (needCode)  trumassege = encodeDecode("12", massege);
    else trumassege = massege;
    //if (str.isEmpty()) trumassege = "No right data";

    //m_ptxt->append("Server send: " + trumassege);
    m_ptxt->append(trumassege);


    //--

    this->socketDescriptor = pClientSocket->socketDescriptor();
    //qDebug()<<"socketDescriptor = " << socketDescriptor;


//проверка пока
    sendToServer(pClientSocket,
                 "Client Response: Received \"" + trumassege + "\"" + " from " + QString::number(this->socketDescriptor)
                 );
    //++
}

// ----------------------------------------------------------------------
void MyClient::sendToServer(QTcpSocket* pSocket, const QString& str)
{
   // qDebug()<<"sendToServer = " << str;
   // qDebug() << "this->needZip "<<this->needZip;
   // qDebug() << "this->needCode "<<this->needCode;
    QString trumassege;
    if  (needCode)  trumassege = encodeDecode("12", str);
    else trumassege = str;
    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    //QDataStream out(&ZiparrBlock, QIODevice::WriteOnly);

    out.setVersion(QDataStream::Qt_5_3);
    //out << quint16(0) << QTime::currentTime() << str;
    out << quint16(0)  << str;
    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));
    QByteArray ZiparrBlock;
    if ((needZip))  ZiparrBlock=qCompress(arrBlock,9);
    else ZiparrBlock = arrBlock;
    pSocket->write(ZiparrBlock);

}

void MyClient::clearTextField()
{
    this->m_ptxt->clear();
}

void MyClient::SetChekBxNeebZip(bool status)
{
     this->needZip = status;
    //qDebug()<<"this->needZip= " << this->needZip;
}

void MyClient::SetChekBxNeebCode(bool status)
{
   this->needCode = status;
  // qDebug()<<"needCode= " << this->needCode;
}

QString MyClient::encodeDecode(QString Stringkey, QString inputStr)
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

//int MyClient::setFasetoheckBox(int fase)
//{
//    switch (fase) {
//    case 1:
//    {
//        this->ChekBxNeebZip->setChecked(0);
//        this->ChekBxNeebCode->setChecked(0);
//    }
//        break;
//    case 2:
//    {
//        this->ChekBxNeebZip->setChecked(1);
//        this->ChekBxNeebCode->setChecked(0);
//    }
//        break;
//    case 3:
//    {
//        this->ChekBxNeebZip->setChecked(0);
//        this->ChekBxNeebCode->setChecked(1);
//    }
//        break;
//    case 4:
//    {
//        this->ChekBxNeebZip->setChecked(0);
//        this->ChekBxNeebCode->setChecked(1);
//    }
//        break;
//    default:
//        break;
//    }

//}
