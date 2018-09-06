#include <QtWidgets>
#include <QtGui>
#include "MyServerClass.h"
#include "MySocket.h"
#include "mythread.h"
#include <QDebug>
#include <QListView>
#include <QPair>
#include <QList>
#include <QSplitter>
#include <QCheckBox>
#include <QFile>
#include <iostream>
//#include <QThread>
#include <QTextStream>
#include <iostream>

//https://habr.com/post/150274/
//https://mayaposch.wordpress.com/2011/11/01/how-to-really-truly-use-qthreads-the-full-explanation/
// ----------------------------------------------------------------------


MyServer::MyServer(const QString& strHost,
                   int            nPort,
                   QWidget*       pwgt /*=0*/
                   ) : QWidget(pwgt)
  , m_nNextBlockSize(0)
{
    GetFile();
    qRegisterMetaType<QTextCursor>("QTextCursor");
    m_ptxtInfo  = new QTextEdit;
    m_ptxtInfo->setReadOnly(true); //на чтение
    WordInfo = new QTextEdit;
    m_ptxtInfo->setReadOnly(true);
    m_ptxtInput = new QLineEdit;
    newPortnumber = new QLineEdit;
    ListWidget = new  QListWidget;
    QPushButton* pcmd = new QPushButton("&Send");
    //this->Port_List.append(nPort);//пока 1порт из main
    connect(this, SIGNAL(addPort()), SLOT(AddNewPort()));
    setupListWidget();
    QString ThreadId = QString( "0x%1" ).arg( (int)QThread::currentThread(), 16 );
    m_ptxtInfo->append("Main tread  addres # " + ThreadId);
    //m_ptxtInfo->setText();
    //++
    connect(this , SIGNAL(SigbalDelPortFromBtm(int)),
            //this,        SLOT(slotSendToClient())
            this, SLOT(DelPort(int))
            );

    connect(pcmd, SIGNAL(clicked()), this, SLOT(slotSendToClient()));
    connect(pcmd, SIGNAL(clicked()), this, SLOT(setMaintext()));

    QPushButton* NewPort = new QPushButton("& + Port");
    connect(NewPort, SIGNAL(clicked()), SLOT(AddNewPort()));

    QPushButton* ClearBtm = new QPushButton("&Clear");
    connect(ClearBtm, SIGNAL(clicked()), SLOT(clearTextField()));

    QPushButton* DelPort = new QPushButton("& - Port");
    connect(DelPort, SIGNAL(clicked()), SLOT(DelPortFromBtm()));

    QPushButton* RandLetter = new QPushButton("Add text from file");
    connect(RandLetter, SIGNAL(clicked()), SLOT(AddnewText()));

    ServerLabel = new  QLabel("<H1>Server</H1>");
    ServerLabel->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    ListLabel = new  QLabel("<H1>Log</H1>");
    ListLabel->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    PortLabel = new  QLabel("<H1>Active Port</H1>");
    PortLabel->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);


    ChekBxNeebZip = new QCheckBox("Сжать");
    ChekBxNeebZip->setChecked(true);
    connect(this->ChekBxNeebZip , SIGNAL(clicked(bool)),
            this, SLOT(GetChekBxNeebZip(bool))
            );


    ChekBxNeebCode= new QCheckBox("Закодировать");
    ChekBxNeebCode->setChecked(true);
    connect(this->ChekBxNeebCode , SIGNAL(clicked(bool)),
            this, SLOT(GetChekBxNeeCode(bool))
            );

    //Layout setup
    QVBoxLayout* MainLayout = new QVBoxLayout;
    MainLayout->addWidget(ServerLabel);

    QHBoxLayout* LabelLayout = new QHBoxLayout;
    LabelLayout ->addWidget(ListLabel);
    LabelLayout ->addWidget(ChekBxNeebZip);
    LabelLayout ->addWidget(ChekBxNeebCode);
    LabelLayout->addStretch(5);
    LabelLayout ->addWidget(PortLabel);

    MainLayout->addLayout(LabelLayout);

    Left2Splitter = new QSplitter(Qt::Horizontal);
    Left2Splitter->addWidget(m_ptxtInfo);
    Left2Splitter->addWidget(ListWidget);

    //WidgetSplitter->addWidget(ListWidget);
    Left2Splitter->setStretchFactor(0, 4);
    Left2Splitter->setStretchFactor(1, 1);


    // MainLayout->addWidget(WidgetSplitter);
    MainLayout->addWidget(Left2Splitter);
    MainLayout->addWidget(WordInfo);
    MainLayout->addWidget(m_ptxtInput);
    QHBoxLayout* newPortLayout = new QHBoxLayout;
    newPortLayout->addWidget(newPortnumber);
    newPortLayout->addWidget(NewPort);
    newPortLayout->addWidget(ClearBtm);
     newPortLayout->addWidget(RandLetter);
    newPortLayout->addWidget(DelPort);

    MainLayout->addLayout(newPortLayout);
    MainLayout->addWidget(pcmd);
    this->setLayout(MainLayout);


}


MyServer::~MyServer()
{

}

void MyServer::slotSendToClient()
{

    //qDebug()<< "MyServer::slotSendToClient()";
    emit SignalSendToClient();
    //m_ptxtInput->setText("");
}

// ------------------------------------------------------------------
void MyServer::slotConnected()
{
    m_ptxtInfo->append("Received the connected() signal");
}

void MyServer::AddNewPort()
{
    int Newnumber;
    if (newPortnumber->text().toInt())
        Newnumber = newPortnumber->text().toInt();
    else if (Port_List.size()>0)
        Newnumber = Port_List.at(Port_List.size()-1) + 1;
    else Newnumber = 6000;
    Port_List.append(Newnumber);
    QString number = QString::number(Newnumber);

   // MySocket * TcpSocket = new MySocket("localhost" , Newnumber, this);
    //TcpSocket_List.append(TcpSocket);
    setupListWidget();
//++
//    connect(this, SIGNAL(SignalSendToClient()),
//            TcpSocket, SLOT(slotSendToClient())//исправить TcpSocket_List.at(0)
//            );
//--
   MyThread *thread  = new MyThread("localhost" , Newnumber, this);

    thread->start();

    m_ptxtInfo->append("Add new Port " + number);
    newPortnumber->clear();
}

void MyServer::clearTextField()
{
    this->m_ptxtInfo->clear();
    this->WordInfo->clear();
}

void MyServer::DelPort(int port)
{
    int Portnumber;
    //qDebug()<< "Portnumber " <<port;
    if (port > 0)
        Portnumber = port;
    // qDebug()<< "Portnumber " << Portnumber;
    else  Portnumber = ListWidget->selectionModel()->currentIndex().data(Qt::DisplayRole).toInt();
    //Portnumber = ListWidget->selectionModel()->currentIndex().data(Qt::DisplayRole).toInt();
    //qDebug()<< "DelPort(int port) " <<Portnumber;
    for (int i =0;i< Port_List.size();i++)
    {
        if (Port_List.at(i) == Portnumber)
        {
//            disconnect(this, SIGNAL(SignalSendToClient()),
//                       TcpSocket_List.at(i), SLOT(slotSendToClient()));


            //emit SigbalDelTread(Port_List.at(i));
            emit SigbalDelPortFromBtm2(Port_List.at(i));
            RemovePortFromList(i);
            //setupListWidget();
            m_ptxtInfo->append("Port #" + QString::number(Portnumber) + " removed from PortList");

            //disconnect(SignalSendToClient(),)
            break;
        }

    }
//qDebug()<< "End DelPort ";
}

void MyServer::test()
{
    qDebug()<< "test()";
}

void MyServer::DelPortFromBtm()
{
    //qDebug()<< "DelPortFromBtm";
    emit SigbalDelPortFromBtm(-1);
}

void MyServer::TextPrint(QString str)
{
    this->m_ptxtInfo->append(str);
}

void MyServer::GetChekBxNeebZip(bool status)
{
    //qDebug()<< "emit ChekBxNeebZipChange(status)";
    emit ChekBxNeebZipChange(status);
}

void MyServer::GetChekBxNeeCode(bool status)
{
     emit ChekBxNeebCodeChange(status);
}

QTextEdit *MyServer::getTextEdit()
{
    return this->m_ptxtInfo;
}

QLineEdit *MyServer::getQLineEdit()
{
    return this->m_ptxtInput;
}

bool MyServer::getCheckStatus()
{
    return this->ChekBxNeebZip->isChecked();
}

bool MyServer::getCheckCodeStatus()
{
    return this->ChekBxNeebCode->isChecked();
}
void MyServer::setupListWidget()
{
    ListWidget->clear();
    for (int i =0;i< Port_List.size();i++)
    {
        ListWidget->insertItem(0, QString("%1").arg(Port_List.at(i)));
    }
    ListWidget->sortItems(Qt::AscendingOrder);
    ListWidget->setSortingEnabled(true);
}

void MyServer::RemovePortFromList(int port)
{
    Port_List.removeAt(port);
    setupListWidget();
}

void MyServer::setMaintext()
{
    this-> WordInfo->append(this->m_ptxtInput->text());
}

void MyServer::AddnewText()
{
   int i = randomBetween(1, WordList.size());
   //qDebug()<< "randomBetween " <<i;
   //int i = 2;
   //m_ptxtInput->setText(WordList2.at(i));
   // qDebug()<< "randomBetween " <<i;
  m_ptxtInput->setText(WordList.at(i));
    //qDebug()<< "AddnewText() " << WordList2.at(i);
}

int MyServer::randomBetween(int low, int high)
{
    return (qrand() % ((high + 1) - low) + low);
}

void MyServer::GetFile()
{
     //qDebug()<<"GetFile()";
//    QString path;
//   // QString path = QDir::currentPath();
//    //QString path= "C:/Users/User/Desktop/TCP_Niit/MyServerTCP"
//    path = "C:/Users/User/Desktop/TCP_Niit/MyServerTCP";
//    QString FolderPath = "/sourse/";
//    QString FilePath = "Setting.txt";
//    //qDebug()<<path;
//    QDir dir;

//    QFile file (path +FolderPath + FilePath);

//    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate |  QIODevice::ReadWrite)) {
//        //file.open(QIODevice::Truncate);
//        file.close();
//    }
//    else if (!dir.exists(path + FolderPath))
//    {
//        //qDebug()<<"dir.exists";
//        dir.mkpath(path + FolderPath);
//        //qDebug()<<" directory "<< path +"/sourse/" <<" now exists";
//        if ( file.open(QIODevice::ReadWrite) )
//        {
//            //qDebug()<<"file now exists";
//        }
//    }

//    file.open(QIODevice::ReadOnly);
//    {

//    //QFile file("Setting.txt");
//    //if (file.exists()) qDebug()<< "file->exists()" ;
////    if (file.open(QIODevice::ReadOnly)) {
//        QTextStream in(&file);
//        while (!in.atEnd()) {
//            QString line = in.readLine();
//            if (line.isEmpty()) continue;
//            this->WordList = line.split(' ', QString::SkipEmptyParts);
//           // this->WordList2.append(line);
//            qDebug()<< line << " ";
//            //qDebug()<< line << " ";
//        }
//    }
//    file.close();

    QFile file("Setting.txt");
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line.isNull() || line.isEmpty()) continue;
            //QStringList coords = line.split(' ',
//            WordList = line.split(' ',
//                                            QString::SkipEmptyParts);
            this->WordList.append(line);

           // qDebug()<< line;
        }
    }
}



