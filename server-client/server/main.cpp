#include <QCoreApplication>
#include "MyTcpServer.h"
#include "../mylib/ConsoleHelper.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Установка обработчика для QDebug и т.д.
    qInstallMessageHandler(ConsoleHelper::messageHandler);

    // Вывод сигнала из потока в главный поток
    qRegisterMetaType<size_t>("size_t");

    // Запуск сервера
    MyTcpServer server("config_server.ini");
    server.server_run();

    return a.exec();
}

