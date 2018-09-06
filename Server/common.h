#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <cstdlib>
#include <QFile>
#include <cstring>
#include <QDateTime>
#include <QString>
#include <iostream>
#include <vector>
#include "QTime"
using namespace std;

struct Param
{
    int port;
    bool compression;
    bool encryption;
};

#endif // COMMON_H
