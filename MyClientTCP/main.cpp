
#include <QtWidgets>
#include "MyClient.h"

// ----------------------------------------------------------------------
int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    app.setApplicationName("Client");
    MyClient     Client("localhost",6000);
    MyClient     Client2("localhost",6001);
    MyClient     Client3("localhost",6002);

    Client.move(0,0);
    Client2.move(400,0);
    Client3.move(800,0);
    Client.show();
    Client2.show();
    Client3.show();
    return app.exec();
}
