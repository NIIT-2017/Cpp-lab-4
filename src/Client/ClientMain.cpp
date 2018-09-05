#pragma once
#include "../../include/Client/Client.h"
extern SOCKET Connection;

int main(int argc, char ** argv)
{

	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 1);
	if (WSAStartup(DLLVersion, &wsaData) != 0) {
		std::cout << "Error" << std::endl;
		exit(1);
	}

	std::string name;
	SOCKADDR_IN addr;
	int sizeofAddr = sizeof(addr);
	if (argc > 1)// если передаем аргументы, то argc будет больше 1(в зависимости от кол-ва аргументов)
	{
		std::cout << argv[1] << std::endl;// вывод второй строки из массива указателей на строки(нумерация в строках начинается с 0 )
		std::cout << argv[2] << std::endl;
		addr.sin_addr.s_addr = inet_addr(argv[1]);
		addr.sin_port = htons(atoi(argv[2]));
		addr.sin_family = AF_INET;
		name = argv[3];
	}
	else
	{
		std::cout << "Not arguments" << std::endl;
		addr.sin_addr.s_addr = inet_addr("127.0.0.1");
		addr.sin_port = htons(1111);
		addr.sin_family = AF_INET;
		name = "noname";
	}
	int name_size = name.size();
	
	Connection = socket(AF_INET, SOCK_STREAM, NULL);
	if (connect(Connection, (SOCKADDR*)&addr, sizeofAddr))
	{
		std::cout << "Error: failed connect to server!\n";
		return 1;
	}
	std::cout << "Connected!\n";
	send(Connection, (char*)&name_size, sizeof(int), NULL);
	send(Connection, name.c_str(), name_size, NULL);
	std::thread client_thread(clientHandler);
	client_thread.join();
	closesocket(Connection);
	WSACleanup();
}