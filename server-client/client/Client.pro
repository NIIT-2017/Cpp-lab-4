QT += core network
QT -= gui

TARGET = Client
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

LIBDIR += "/usr/lib"
LIBS += -L$${LIBDIR} -lz -lcrypto

HEADERS += \
    MyTcpClient.h \
    ../mylib/Connection.h \
    ../mylib/ConsoleHelper.h \
    ../mylib/Message.h \
    ../mylib/MessageType.h \
    ../mylib/MyTcpApp.h

SOURCES += \
    main.cpp \
    MyTcpClient.cpp \
    ../mylib/Connection.cpp \
    ../mylib/ConsoleHelper.cpp \
    ../mylib/Message.cpp \
    ../mylib/MyTcpApp.cpp
    ../mylib/MyTcpApp.cpp

OTHER_FILES += \
    config_client.ini

for(FILE, OTHER_FILES) {
    system(cp $$PWD/$$FILE $$OUT_PWD);
}
