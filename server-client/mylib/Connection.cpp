#include "Connection.h"
#include "MyTcpApp.h"
#include <QThread>


Connection::Connection(QTcpSocket *socket, QObject * parent) : QObject(parent), configMessage(ConfigMessage())
{
    this->socket = socket;
    connect(socket, &QTcpSocket::disconnected, this, &Connection::disconnected);
}

Connection::~Connection()
{
    socket->close();
}

/* Отправка сообщения */
void Connection::send(Message &message, bool info)
{
    QString dataLog = nullptr;
    message.setInfo(info);
    message.setConfigMessage(configMessage);
    // Переводим сообщение в поток байтов
    QByteArray *data = message.writeObject();
    // Если сообщение с именем или данными, то сначала отправляем сообщение с размером передаваемых данных
    if (message.getType() == MessageType::USER_NAME || message.getType() == MessageType::DATA)
    {
        // Формирование и отправка сообщения с размером данных
        dataLog = *message.getData();
        QString *str = new QString(QString::number(data->length()));
        Message mes(MessageType::SIZE_DATA, str, configMessage, info);
        QByteArray *d = mes.writeObject();
        socket->write(*d);
        delete d;
        // Ожидание ответа о готовности получения данных
        Message *mes2 = receive(info);
        if (mes2->getType() != MessageType::READY)
        {
            qCritical() << "Connection::send::\tmessage sending fail";
            return;
        }
        delete mes2;
    }
    // Отправка основного сообщения
    socket->write(*data);
    // Просто логфайл
    if (dataLog != nullptr)
    {
        qDebug().nospace() << "Connection::send::   \t" << clientThread->getFullName()
                           << " text: " << dataLog;
    }
    else
    {
        qDebug().nospace() << "Connection::send::   \t" << clientThread->getFullName()
                           << " text: " << QString::fromUtf8(*data);
    }
    delete data;
}

/* Отправка сообщения с приостоновлением события приема данных "QTcpSocket::readyRead" */
/* Это необходимо, когда отправляем данные, потому что в этом случае нужно самим получить готовность к приему */
void Connection::sendB(Message &message, bool b)
{
    if (b)
    {
        runLoop(false);
        send(message);
        runLoop(true);
    }
    else send(message);
}

/* Отправка пустого сообщения, но с типом */
void Connection::send(MessageType messageType)
{
    Message message(messageType);
    send(message, true);
}

/* Получение сообщения */
Message * Connection::receive(bool info)
{
    // Ждем данных
    while (!socket->waitForReadyRead(1))
    {
        if (socket->bytesAvailable() > 0) break;
    }
    // Считываем данные в поток байтов
    QByteArray data = socket->readAll();
    // Создаем новое сообщение и загружаем данные в него
    Message *message = new Message(configMessage, info);
    message->readObject(data);
    // Если сообщение по типу не "размер данных", то все получили
    if (message->getType() != MessageType::SIZE_DATA)
    {
        if (message->getType() == MessageType::DATA)
        {
            qDebug().nospace() << "Connection::recieve::\t" << clientThread->getFullName()
                               << " text: " << *message->getData();
        }
        else
        {
            qDebug().nospace() << "Connection::recieve::\t" << clientThread->getFullName()
                               << " text: " << QString::fromUtf8(data);
        }
        return message;
    }

    // Если сообщение по типу "размер данных", то отвечаем о готовности получать данные
    Message mes(MessageType::READY);
    QByteArray *d = mes.writeObject();
    socket->write(*d);
    delete d;
    // Ждем пока в системе не накопится полностью все сообщение
    int size = message->getData()->toInt();
    while (socket->bytesAvailable() < size)
        socket->waitForReadyRead(1);
    delete message;
    // Когда накопится, получаем его
    message = receive(info);
    return message;
}

/* Клиент отсоединился */
void Connection::disconnected()
{
    clientThread->clientDisconnect();
}

/* Переход в основную петлю */
void Connection::mainLoop()
{
    clientThread->clientLoop();
}

QTcpSocket * Connection::getRemoteSocket()
{
    return socket;
}

void Connection::setClientThread(MyTcpApp *clientThread)
{
    this->clientThread = clientThread;
}

void Connection::setConsoleHelper(ConsoleHelper *consoleHelper)
{
    this->consoleHelper = consoleHelper;
}

void Connection::setConfigMessage(ConfigMessage config)
{
    configMessage = config;
}

/* Установка и выключение сигнала о готовности получения */
/* Необходимо для логики отправки сообщения данных */
/* (там нужно получить сообщение о готовности принятия данных) */
void Connection::runLoop(bool b)
{
    if (b) connect(socket, &QTcpSocket::readyRead, this, &Connection::mainLoop);
    else  disconnect(socket, &QTcpSocket::readyRead, this, &Connection::mainLoop);
}
