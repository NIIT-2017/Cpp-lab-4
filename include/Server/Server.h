#pragma once
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <thread>
#include <iostream>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <string>
#include <list>
#include <set>
#include <map>
#include <mutex>
#include <fstream>
#include <zlib.h>
#include <cstdlib>

#pragma warning(disable: 4996)
struct ServerConfig {
	SOCKADDR_IN addr;
	int transmode;
};
int fill(ServerConfig & cnf, const char * str1, const char * str2, int count);
bool fillConfig(ServerConfig & cnf, const std::string & path);
std::map<int, std::string> fillAforisms(const std::string & path);
char * crypt(const char * information, unsigned long length);