
#include <QApplication>
#include "MySocket.h"
#include "MyServerClass.h"
// ----------------------------------------------------------------------
int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    Q_INIT_RESOURCE(qrc);
    app.setApplicationName("Server");
    MyServer  Server("localhost", 6000);

    Server.show();

    return app.exec();
}
