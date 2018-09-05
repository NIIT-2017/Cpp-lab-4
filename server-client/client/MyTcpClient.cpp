#include <QDebug>
#include <QCoreApplication>

#include "MyTcpClient.h"
#include "../mylib/Connection.h"

MyTcpClient::MyTcpClient(QString log, QString ipCons, QString nameCons) : configMessage(ConfigMessage()), consoleHelper("log_client_")
{
    // Загружаем данные из конфигурационного файла
    QFile file(log);
    if (!file.exists() || !file.open(QIODevice::ReadOnly))
    {
        consoleHelper.write(qCritical(), "MyTcpClient:: client not found config file");
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
    if (config.contains("name"))
        name = config.value("name");
    else
        name = "Anonimous";
    if (nameCons != nullptr) name = nameCons;
    if (config.contains("ip"))
        serverIP = config.value("ip");
    else
        serverIP = "127.0.0.1";
    if (ipCons != nullptr) serverIP = ipCons;
    if (config.contains("port"))
        serverPort = static_cast<quint16>(config.value("port").toUInt());
    else
        serverPort = 6000;
}

/* Запускаем клиент */
void MyTcpClient::client_run()
{
    // Соединение с сервером
    socket = new QTcpSocket(this);
    socket->connectToHost(serverIP, serverPort);
    if (socket->waitForConnected(10000))
    {
        fullName += "server_ip=" + serverIP + " port=" + QString::number(socket->peerPort())
                + " user_name=" + name;
        consoleHelper.write(qInfo(), "MyTcpClient:: client started");
        connection = new Connection(socket);
        connection->setClientThread(this);
        connection->setConsoleHelper(&consoleHelper);
        // Соединяемся с сервером и проводим все процедуры идентификации
        if (!handshake())
        {
            consoleHelper.write(qWarning(), "MyTcpClient:: client did not connected");
            clientDisconnect();
        }
        // Устанавливаем сигнал дисконнекта
        connect(socket, &QTcpSocket::disconnected, this, &MyTcpClient::clientDisconnect);
        qInfo().nospace() << "MyTcpClient:: connected to server " << fullName;
    }
    else
    {
        consoleHelper.write(qCritical(), "MyTcpClient:: client is not started");
        clientDisconnect();
    }

    // Запускаем сигнал прослушивания порта
    serverConnected = true;
    connection->runLoop(true);
    QString str('\0');

    // Для задания, получить один афоризм
    // Отправили запрос
    Message message(MessageType::DATA, new QString(str));
    connection->sendB(message, true);
    // Ждем афоризма в течение 1 cекунды
    socket->waitForReadyRead(1000);
    if (connection->getRemoteSocket()->bytesAvailable() > 0)
        clientLoop();

/*
    // Петля для написания сообщений для сервера
    // Можно получать афоризмы снова и снова, пока не будет написана фраза "exit" или нажата ctrl+c
    // Можно модернизировать в полный диалог с сервером, переведя в отдельный поток прослушивание порта
    while (serverConnected)
    {
        if (str == "exit") break;
        else
        {
            // Отправили сообщение серверу
            Message message(MessageType::DATA, new QString(str));
            connection->sendB(message, true);
            // Ждем афоризма в течение 500 мс
            socket->waitForReadyRead(500);
            if (connection->getRemoteSocket()->bytesAvailable() > 0) clientLoop();
        }
        // Ввод с клавиатуры
        str = consoleHelper.read();
    }
*/
    clientDisconnect();
}

/* Соединение с сервером и проведение всех процедур идентификации */
bool MyTcpClient::handshake()
{
    // Получение запроса имени от сервера
    Message *message = connection->receive(true);
    if (message->getType() != MessageType::NAME_REQUEST)
        return false;
    delete message;
    // Отправка имени клиента
    message = new Message(MessageType::USER_NAME, new QString(name), configMessage, true);
    connection->send(*message);
    delete message;
    // Получение подтверждения идентификации
    message = connection->receive(true);
    if (message->getType() != MessageType::NAME_ACCEPTED)
    {
        delete message;
        return false;
    }
    delete message;
    // Отправка сообщения об ожидании получения конфигурации сервера
    connection->send(MessageType::READY);
    // Получение и расшифровка конфигурации сервера
    message = connection->receive(true);
    if (message->getType() != MessageType::CONFIG)
    {
        delete message;
        return false;
    }
    QString *data = message->getData();
    int k = (*data).indexOf('@');
    int c = (*data).mid(0, k).toInt();
    configMessage.isCompression = c % 10;
    configMessage.isEncryption = c / 10;

    if (configMessage.isEncryption)
    {
        // Получаем публичный ключ сервера
        k++;
        *data = data->mid(k);
        QByteArray *public_key = new QByteArray(data->toUtf8());
        Message::GenRsaFromPublicKey(&configMessage.rsa_public, public_key);
        delete public_key;
        // Формируем ключи клиента
        Message::GenRsa(&configMessage.rsa_private);
        // Отправляем приватный ключ серверу
        public_key = nullptr;
        QString *str = new QString();
        Message::GenPublicKeyFromRsa(configMessage.rsa_private, &public_key);
        *str += QString::fromUtf8(*public_key);
        delete public_key;
        message = new Message(MessageType::CONFIG, str);
        connection->send(*message, true);
        delete message;
        // Получение сигнала ожидания
        message = connection->receive(true);
        if (message->getType() != MessageType::READY)
        {
            delete message;
            return false;
        }
    }
    connection->setConfigMessage(configMessage);
    delete message;
    return true;
}

/* Отсоединение клиента */
void MyTcpClient::clientDisconnect()
{
    connection->deleteLater();
    serverConnected = false;
    qInfo().nospace() << "MyTcpClient:: disconnected " << fullName;
    consoleHelper.write(qInfo(), "MyTcpClient:: client finished");
    exit(0);
}

/* Получение сообщений от сервера */
void MyTcpClient::clientLoop()
{
    Message *message = connection->receive();
    if (message->getType() == MessageType::DATA)
    {
        consoleHelper.write(*message->getData());
    }
}

QString MyTcpClient::getFullName() const
{
    return fullName;
}
