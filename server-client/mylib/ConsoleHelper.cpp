#include "ConsoleHelper.h"

#include <QtDebug>
#include <QDateTime>


QMutex ConsoleHelper::mutex;
QFile *ConsoleHelper::logfile;

/* Слушатель сообщений для ведения глобального лога */
void ConsoleHelper::messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QMutexLocker locker(&mutex);
    QTextStream out(logfile);
    out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ");
    switch (type)
    {
    case QtInfoMsg:     out << "INF "; break;
    case QtDebugMsg:    out << "DBG "; break;
    case QtWarningMsg:  out << "WRN "; break;
    case QtCriticalMsg: out << "CRT "; break;
    case QtFatalMsg:    out << "FTL "; break;
    }
    out << context.category << ": " << msg << endl;
    out.flush();
}

ConsoleHelper::ConsoleHelper(QString logf) : cin(stdin), cout(stdout)
{
    logf += QDateTime::currentDateTime().toString("yyyy-MM-dd");
    logf += ".txt";
    logfile = new QFile(logf);
    logfile->open(QFile::Append | QFile::Text);
}

ConsoleHelper::~ConsoleHelper()
{
    logfile->close();
    delete logfile;
}

/* чтение с экрана */
QString ConsoleHelper::read()
{
    QString line = cin.readLine();
    return line;
}

/* Запись в глобальный лог файл и на экран */
void ConsoleHelper::write(QDebug qml, const QString &str)
{
    QMutexLocker locker(&mutex);
    cout << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ");
    cout << str << endl;
    cout.flush();
    qml << str;
}

/* Запись в глобальный лог файл */
void ConsoleHelper::write(const QString &str)
{
    QMutexLocker locker(&mutex);
    cout << str << endl;
    cout.flush();
}
