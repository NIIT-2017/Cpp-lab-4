#include "client.h"

QByteArray decodeStr(const QByteArray &str)
{
    quint32 key = 073; //шифр
    QByteArray arr = str;
    for(int i =0; i<arr.size(); i++)
        arr[i] =arr[i] ^ key;

    return arr;
}

Client::Client(char *name)
{

    string nstr(name);
    nstr="SettingsClient.txt";

    if (read1(nstr)==true)
    {
    socket=new QTcpSocket (this);
    connect(socket, SIGNAL(readyRead()), this, SLOT(sockReady()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(sockDisc()));

    QString qstr=QString::fromStdString(param.adress);
    socket->connectToHost(qstr, param.port);

    QTime midnight(0,0,0);
    qsrand(midnight.secsTo(QTime::currentTime()));
    int id=qrand();
    QString qst=QString::number(id);
    QByteArray buffer=qst.toLocal8Bit();
    socket->write(buffer);
    }
}

void Client::sockReady()
{
    if (socket->waitForConnected(500))
    {


        socket->waitForReadyRead(500);
        QByteArray Data=socket->readAll();
        if (param.decompression==true)Data=qUncompress(Data);
        if (param.decoding==true)Data=decodeStr(Data);

        QString qst(Data);
        string result=qst.toStdString();

        cout << result << endl;
    }
}

void Client::sockDisc()
{
    socket->deleteLater();
}



bool Client::read1(string nstr)
{
    string s1;
    char *str1, *str2;
    int k=1;


        QFile file(QString::fromStdString(nstr));

        if ((file.exists())&&(file.open(QIODevice::ReadOnly)))
        {
            QString str="";
            while(!file.atEnd())
            {
                str=file.readLine();

                s1=str.toStdString();

                if (s1.size()==0) continue;

                str1=strtok((char*)s1.c_str(), "=");

                str2 = str1;
                str1 = strtok(NULL, "\n");

                if (str1 != NULL)
                    {

                    if (fill(str2, str1, k) != k)
                         {
                             cout << "Error reading data" << endl;
                             return false;
                         }
                       k++;
                    }
            }

            if (k != 5) { cout << "Error reading data" << endl; return false;}
            file.close();
        }
        else
        {
            qDebug() << "Error opening file";
            return false;
        }

    return true;
}

int Client::fill(char *a, char *b, int k)
{

    if (k == 1 && strcmp(a, "IP_adress") == 0)
    {
        string f (b);
        param.adress = f;
        return k;
    }

    if (k == 2 && strcmp(a, "Port") == 0)
    {
        param.port = atoi(b);
        return k;
    }

    if (k == 3 && strcmp(a, "Decompression") == 0)
    {
        int t=atoi(b);
        if (t<2 && t>=0)
        {
          param.decompression = t;
          return k;
        }
    }

    if (k == 4 && strcmp(a, "Decoding") == 0)
    {
        int t=atoi(b);
        if (t<2 && t>=0)
        {
          param.decoding = t;
          return k;
        }
    }

    return 0;
}


