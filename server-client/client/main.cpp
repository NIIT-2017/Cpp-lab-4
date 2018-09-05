#include <QCoreApplication>
#include "MyTcpClient.h"
#include "../mylib/ConsoleHelper.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    if (argc != 1 && argc != 3)
    {
        return 1;
    }

    // Установка обработчика для QDebug и т.д.
    qInstallMessageHandler(ConsoleHelper::messageHandler);

    // Запуск клиента
    if (argc == 1)
    {
        MyTcpClient client("config_client.ini");
        client.client_run();
    }
    else
    {
        MyTcpClient client("config_client.ini", QString(argv[1]), QString(argv[2]));
        client.client_run();
    }

    return a.exec();
}
