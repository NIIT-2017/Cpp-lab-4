QT += core network
QT -= gui

TARGET = Server
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

LIBDIR += "/usr/lib"
LIBS += -L$${LIBDIR} -lz -lcrypto

HEADERS += \
    ClientThread.h \
    MyTcpServer.h \
    ../mylib/Connection.h \
    ../mylib/ConsoleHelper.h \
    ../mylib/Message.h \
    ../mylib/MessageType.h \
    ../mylib/MyTcpApp.h

SOURCES += \
    ClientThread.cpp \
    main.cpp \
    MyTcpServer.cpp \
    ../mylib/Connection.cpp \
    ../mylib/ConsoleHelper.cpp \
    ../mylib/Message.cpp \
    ../mylib/MyTcpApp.cpp

OTHER_FILES += \
    config_server.ini \
    ../aphorisms.txt

for(FILE, OTHER_FILES) {
    system(cp $$PWD/$$FILE $$OUT_PWD);
}
