#include "MyTcpServer.h"

#include <QDebug>
#include <QCoreApplication>
#include <QFile>
#include <QRegExp>
#include <QThreadPool>
#include <QTime>

ConsoleHelper *MyTcpServer::consoleHelper;
QList<QString> MyTcpServer::aphorisms;
int MyTcpServer::countAphorisms;
QFile *MyTcpServer::logFileSolution;    //---------- нужно только для условия задачи про логфайл ----------//
QTextStream *MyTcpServer::logSolution;  //---------- нужно только для условия задачи про логфайл ----------//
QMutex MyTcpServer::mutex;              //---------- нужно только для условия задачи про логфайл ----------//


MyTcpServer::MyTcpServer(QString log, QObject *parent) : QTcpServer(parent)
{
    consoleHelper = new ConsoleHelper("log_server_");

    // Загружаем данные из конфигурационного файла
    QFile file(log);
    if (!file.exists() || !file.open(QIODevice::ReadOnly))
    {
        consoleHelper->write(qCritical(), "MyTcpServer:: server not found config file");
        exit(1);
    }

    QString str;
    while(!file.atEnd())
    {
        str = file.readLine();
        str = str.remove(QRegExp("[\t\\s\r\n]"));
        if (str[0] == '#') continue;
        if (str.size() == 0) continue;
        QStringList list = str.split(":");
        config.insert(list[0], list[1]);
    }
    file.close();

    if (config.contains("port"))
        serverPort = static_cast<quint16>(config.value("port").toUInt());
    else
        serverPort = 6000;
    if (config.contains("maxUsers"))
        maxUsers = config.value("maxUsers").toInt();
    else
        maxUsers = 100;
    bool isCompression = false;
    if (config.contains("compression"))
        isCompression = config.value("compression") == "true" ? true : false;
    bool isEncryption = false;
    if (config.contains("encryption"))
        isEncryption = config.value("encryption") == "true" ? true : false;
    configMessage = ConfigMessage(isCompression, isEncryption);
    QThreadPool::globalInstance()->setMaxThreadCount(maxUsers);
    QString fname;
    if (config.contains("datafile"))
        fname = config.value("datafile");
    else
        fname = "aphorisms.txt";

    // Загружаем афоризмы
    QFile file2(fname);
    if (!file2.exists() || !file2.open(QIODevice::ReadOnly))
    {
        consoleHelper->write(qCritical(), "MyTcpServer:: server not found data file with aphorisms");
        exit(1);
    }
    countAphorisms = 0;
    while(!file2.atEnd())
    {
        str = file2.readLine();
        if (str[0] == '#' || str == "\r\n" || str == "\n") continue;
        QString str2 = str;
        while(!file2.atEnd() && (str = file2.readLine()) != "\r\n" && str != "\n")
        {
            if (str[0] == '#') continue;
            str2 += str;
            str = file2.readLine();
        }
        aphorisms.push_back(str2);
        countAphorisms++;
    }
    file.close();

    //---------- Нужно только для условия задачи про логфайл ----------//
    logFileSolution = new QFile("log_solution" + QDateTime::currentDateTime().toString("yyyy-MM-dd") + ".txt");
    logFileSolution->open(QFile::Append | QFile::Text);
    logSolution = new QTextStream(logFileSolution);
}

/* Запускаем сервер, и прослушивание порта сервером */
void MyTcpServer::server_run()
{
    if(!listen(QHostAddress::Any, serverPort))
    {
        consoleHelper->write(qCritical(), "MyTcpServer:: server is not started");
        close();
        exit(2);
    }
    else consoleHelper->write(qInfo(), "MyTcpServer:: server running");
}

MyTcpServer::~MyTcpServer()
{
    for(size_t key : usersMap.keys())
    {
         ClientThread *ct = usersMap.value(key);
         usersMap.remove(id);
         delete ct;
         countUsers--;
    }
    consoleHelper->write(qInfo(), "MyTcpServer:: server stopped");
    logFileSolution->close();
    delete logFileSolution;
    delete logSolution;
    exit(0);
}

/* Основная часть сервера, получаем входящий коннект */
void MyTcpServer::incomingConnection(qintptr socketDescriptor)
{
    // Проверяем на максимум подключенных пользователей
    while (countUsers >= maxUsers)
    {
        consoleHelper->write(qWarning(), "MyTcpServer:: number of connections exceeded");
        QTcpSocket socket;
        socket.setSocketDescriptor(socketDescriptor);
        socket.close();
        return;
    }
    // Ищем свободный id
    while (usersMap.contains(++id)) {}
    countUsers++;
    // Для нового пользователя создаем отдельный поток в глобальном пуле
    ClientThread *ct = new ClientThread(id, socketDescriptor, configMessage);
    usersMap.insert(id, ct);
    connect(ct, SIGNAL(finished(size_t)), this, SLOT(killClient(size_t)));
    QThreadPool::globalInstance()->start(ct);
}


/* Убить поток клиента и убрать из журнала соединений */
void MyTcpServer::killClient(size_t id)
{
    usersMap.remove(id);
    countUsers--;
}
