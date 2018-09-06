#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <cstdlib>
using namespace std;

struct Param
{
    string adress;
    int port;
    bool compression;
    bool encryption;
};

#endif // COMMON_H
