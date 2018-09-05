#ifndef CONSOLEHELPER_H
#define CONSOLEHELPER_H

#include <QString>
#include <QTextStream>
#include <QFile>
#include <QScopedPointer>
#include <QMutex>

class ConsoleHelper
{
private:
    QTextStream cin;
    QTextStream cout;
    static QFile *logfile;
    static QMutex mutex;

public:
    static void messageHandler(QtMsgType, const QMessageLogContext &, const QString &);

    ConsoleHelper(QString);
    ~ConsoleHelper();
    QString read();
    void write(const QString &);
    void write(QDebug, const QString &);
};


#endif // CONSOLEHELPER_H
