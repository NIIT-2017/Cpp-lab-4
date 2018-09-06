#include <QCoreApplication>
#include "client.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //if (argc!=3) return 1;

    //Client client(argv[1]);
    Client client("argv[1]");

    return a.exec();
}
