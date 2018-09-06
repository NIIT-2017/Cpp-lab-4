#include <QCoreApplication>
#include <qsimpleserver.h>


using namespace std;


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

   QSimpleServer server;


    return a.exec();
}






