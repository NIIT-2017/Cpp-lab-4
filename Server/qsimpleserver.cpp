#include "qsimpleserver.h"

QSimpleServer::QSimpleServer()
{
  if (read1()==true && read2()==true)
  {
      if (this->listen(QHostAddress::Any, param.port))
      {
          qDebug()<< "Listening";
      }
      else
      {
          qDebug()<< "Not listening";
      }
  }

}

void QSimpleServer::incomingConnection(qintptr handle)
  {
        QTime midnight(0,0,0);
        qsrand(midnight.secsTo(QTime::currentTime()));
        string str=vstr.at(qrand() % (vstr.size()-1));

    QSocketThread *thread=new QSocketThread(handle, &param, str);
    connect(thread, SIGNAL(finished()), thread, SLOT (deleteLater()));
    thread->start();
}

bool QSimpleServer::read1()
{
    string s1;
    char *str1, *str2;
    int k=1;


        QFile file("SettingsServer.txt");

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

            if (k != 4) { cout << "Error reading data" << endl; return false;}
            file.close();
        }
        else
        {
            qDebug() << "Error opening file";
            return false;
        }

    return true;
}

int QSimpleServer::fill(char *a, char *b, int k)
{

    if (k == 1 && strcmp(a, "Port") == 0)
    {
        param.port = atoi(b);
        return k;
    }

    if (k == 2 && strcmp(a, "Compression") == 0)
    {
        int t=atoi(b);
        if (t<2 && t>=0)
        {
          param.compression = t;
          return k;
        }
    }

    if (k == 3 && strcmp(a, "Encryption") == 0)
    {
        int t=atoi(b);
        if (t<2 && t>=0)
        {
          param.encryption = t;
          return k;
        }
    }

    return 0;
}

bool QSimpleServer::read2()
{
    string s1;
    QFile file("Aphorisms.txt");



    if ((file.exists())&&(file.open(QIODevice::ReadOnly)))
    {
        QString str="";
        while(!file.atEnd())
        {
            str=file.readLine();
            s1=str.toStdString();
            if (s1.size()==2) continue;

            vstr.push_back(s1);
        }

        file.close();
    }
    else
    {
        qDebug() << "Error opening file";
        return false;
    }


    return true;
}
