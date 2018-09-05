#include "ClientThread.h"
#include "MyTcpServer.h"
#include <QEventLoop>
#include <QTime>
#include <QCoreApplication>


ClientThread::ClientThread(size_t id, qintptr socketDescriptor, ConfigMessage configMessage) : configMessage(configMessage)
{
    this->id = id;
    this->socketDescriptor = socketDescriptor;
    if (configMessage.isEncryption)
        Message::GenRsa(&this->configMessage.rsa_private);
}

/* Основная часть работы с клиентом */
void ClientThread::run()
{    
    qsrand(static_cast<uint>(QTime(0,0,0).secsTo(QTime::currentTime())));
    // Создаем петлю событий для потока
    QEventLoop loop;

    // Создаем сокет
    QTcpSocket *socket = new QTcpSocket();
    socket->setSocketDescriptor(socketDescriptor);

    // Создаем коннект, в котором определены функции работы с клиентом
    Connection *connection = new Connection(socket);
    connection->setConfigMessage(configMessage);
    this->connection = connection;
    connection->setClientThread(this);

    fullName += "ip=" + connection->getRemoteSocket()->peerAddress().toString()
            + " port=" + QString::number(connection->getRemoteSocket()->peerPort());
    qInfo() << "ClientThread:: new user connecting";

    // Соединяемся с клиентом и проводим все процедуры идентификации
    if (!handshake())
    {
        qWarning() << "ClientThread:: user connecting fail";
        clientDisconnect();
    }
    fullName += " user_id=" + QString::number(id) + " user_name=" + name;
    qInfo().nospace() << "ClientThread:: connected   " << fullName;
    clientConnected = true;
    // Устанавливаем сигналы окончания соединения
    connect(this, SIGNAL(finished(size_t)), &loop, SLOT(quit()));
    // Устанавливаем сигнал прослушивания порта
    connection->runLoop(true);
    loop.exec();
}

/* Соединение с клиентом и проведение всех процедур идентификации */
bool ClientThread::handshake()
{
    // Запрос имени
    connection->send(MessageType::NAME_REQUEST);
    // Получение ответа
    Message *message = connection->receive(true);
    if (message->getType() != MessageType::USER_NAME)
    {
        delete message;
        return false;
    }
    name = *message->getData();
    delete message;
    // Подтверждение получения имени
    connection->send(MessageType::NAME_ACCEPTED);
    // Получение сигнала ожидания
    message = connection->receive(true);
    if (message->getType() != MessageType::READY)
    {
        delete message;
        return false;
    }
    delete message;
    // Отправка конфигурации сервера (компресия, шифрование, публичный ключ)
    int c = (configMessage.isCompression ? 1 : 0) + (configMessage.isEncryption ? 1 : 0) * 10;
    QString *str = new QString();
    *str = QString::number(c);

    if (configMessage.isEncryption)
    {
        *str += '@';
        // Формируем приватный ключ для отправки клиенту и добавляем к сообщению
        QByteArray *public_key;
        Message::GenPublicKeyFromRsa(configMessage.rsa_private, &public_key);
        *str += QString::fromUtf8(*public_key);
        delete public_key;
    }
    message = new Message(MessageType::CONFIG, str);
    connection->send(*message, true);
    delete message;
    // Получение публичного ключа от клиента
    if (configMessage.isEncryption)
    {
        message = connection->receive(true);
        if (message->getType() != MessageType::CONFIG)
        {
            delete message;
            return false;
        }
        QByteArray *public_key = new QByteArray(message->getData()->toUtf8());
        Message::GenRsaFromPublicKey(&configMessage.rsa_public, public_key);
        delete public_key;
        delete message;
        connection->setConfigMessage(configMessage);
        // Отправка сообщения об ожидании получения конфигурации сервера
        connection->send(MessageType::READY);
    }
    return true;
}

/* Если клиент завершил соединение */
void ClientThread::clientDisconnect()
{
    clientConnected = false;
    connection->deleteLater();
    qInfo().nospace() << "ClientThread:: disconnected " << fullName;
    emit finished(id);
}

/* Основная петля сервера: получение сообщения и отправка в ответ афоризма */
/* Легко модернизируется для полного и длительного диалога с клиентом */
void ClientThread::clientLoop()
{
    // Получение сообщения
    Message *message = connection->receive();
    if (message->getType() == MessageType::DATA)
    {
        // Случайный афоризм
        int i = qrand() % MyTcpServer::countAphorisms;
        QString *str = new QString(MyTcpServer::aphorisms[i]);

//---------- Блок кода чисто для выполнения задания про лог файл ----------//
        {
            QMutexLocker locker(&MyTcpServer::mutex);
            QString st;
            st = "user_name=" + name + " Compression=" + (configMessage.isCompression ? "true" : "false")
                    + " Encryption=" + (configMessage.isEncryption ? "true" : "false")
                    + " text: " + MyTcpServer::aphorisms[i];
            *(MyTcpServer::logSolution) << st << endl;
            MyTcpServer::logSolution->flush();
        }
//---------- ----------//
        delete message;
        // отправка афоризма
        message = new Message(MessageType::DATA, str);
        connection->sendB(*message, true);
    }
}

QString ClientThread::getFullName() const
{
    return fullName;
}
