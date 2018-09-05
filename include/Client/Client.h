#pragma once
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <iostream>
#include <thread>
#include <string>
#include <chrono>
#include <mutex>
#include <zlib.h>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma warning(disable: 4996)
extern std::mutex mtx;

char * crypt(const char * information, unsigned long length);
void clientHandler();