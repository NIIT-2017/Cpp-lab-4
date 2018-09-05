#include "../../include/Server/Server.h"

std::mutex mtx;

int main()
{
	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 1); //запрашиваем версию библиотеки winsock

	if (WSAStartup(DLLVersion, &wsaData) != 0) //загружаем библиотеку, 1-й параметр - версия библио, 2-й параметр - ссылку на структуру wsaData
	{
		std::cout << "Error" << std::endl;
		exit(1);
	}
	ServerConfig cnf;
	SOCKADDR_IN addr;
	int sizeofAddr = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //адрес сокета
	addr.sin_port = htons(1111); //порт
	addr.sin_family = AF_INET; //IPv4
	
	fillConfig(cnf, "Config.txt");

	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL); // создаём сокет
	bind(sListen, (SOCKADDR*)&cnf.addr, sizeof(addr)); // привязываем адрес к сокету
	// 1 - сокет, 2 - указатель на структуру SOCKADDR, 3- размер структуры

	// теперь нужно прослушать порт в ожидание присоединия со стороны клиента
	// 1 - порт, по которому можно запустить прослушивание, 2 - максимальное число запросов

	std::cout << "Server is started\n";

	listen(sListen, SOMAXCONN);

	// объявляем новый сокет, чтобы удерживать соединение с клиентом
	std::list<std::thread*> threads;
	std::map<int, std::string>strings = fillAforisms("Aforisms.txt");
	SOCKET newConnection;
	srand(time(0));

	while (1)
	{
		while(1)
		{
			//accept - принимает новое соединение и возвращает дескриптор на него.
			{
				std::lock_guard<std::mutex> lock(mtx); 
				newConnection = accept(sListen, (SOCKADDR*)&addr, &sizeofAddr);
			}
			if (newConnection == 0) {
				std::cout << "Error #2\n";
				exit(2);
			}
			else {
				std::string msg;
				unsigned long msg_size;
				unsigned long msgl_size;
				unsigned char * msgl;
				{
					std::lock_guard<std::mutex> lock(mtx);
					int name_size;
					recv(newConnection, (char*)&name_size, sizeof(int), NULL);
					char * clientname = new char[name_size + 1]{0};
					recv(newConnection, clientname, name_size, NULL);
					std::cout << std::endl <<  clientname <<" connected\n";
					int k = rand() % strings.size();
					msg = strings[k];
					msg_size = msg.size();
					msgl_size = msg_size + msg_size*0.1 + 12;
					switch (cnf.transmode) {
						case 0:
						{
							msgl = new unsigned char[msg_size + 1]{ 0 };
							strcpy((char*)msgl, msg.c_str());
							msgl[msg_size] = 0;
							break;
						}
						case 1:
						{	
							msgl = (unsigned char*)crypt(msg.c_str(), msg.size());
							break;
						}
						case 2:
						{
							msgl = new unsigned char[msgl_size + 1]{ 0 };
							if (Z_OK != compress((Bytef*)msgl, &msgl_size, reinterpret_cast<const unsigned char*>(msg.c_str()), msg.size()))
							{
								std::cout << "Compression error! " << std::endl;
								exit(2);
							}
							break;
						}
						case 3:
						{
							char * cryptmsg = crypt(msg.c_str(), msg.size());
							msgl_size = msg_size + msg_size*0.1 + 12;
							msgl = new unsigned char[msgl_size + 1]{ 0 };
							if (Z_OK != compress((Bytef*)msgl, &msgl_size, reinterpret_cast<const unsigned char*>(cryptmsg), msg.size()))
							{
								std::cout << "Compression error! " << std::endl;
								exit(2);
							}
							delete[] cryptmsg;

							break;
						}
					}
					std::thread * thread = new std::thread([&newConnection, msgl, msgl_size, msg_size, msg, clientname, cnf]() {
																	std::lock_guard<std::mutex> lock(mtx);		
																	std::cout << "Sending a message to <<" << clientname << ">>:\n" << "\"" << msg << "\"" << std::endl << "Transfer mode: " << cnf.transmode << std::endl;
																	send(newConnection, (char*)&cnf.transmode, sizeof(int), NULL);
																	if (cnf.transmode < 2)
																	{
																		send(newConnection, (char*)&msg_size, sizeof(unsigned long), NULL);
																	}
																	else
																	{
																		send(newConnection, (char*)&msgl_size, sizeof(unsigned long), NULL);
																		send(newConnection, (char*)&msg_size, sizeof(unsigned long), NULL);
																	}
																	int total = 0;
																	int n = 0;
																	while (msgl_size > total)
																	{
																		n = send(newConnection, (char*)(msgl + total), msgl_size - total, NULL);
																		if (n == -1) { break; }
																		total += n;
																	}
																	delete[] msgl;
																	delete[] clientname;
				});
					
					threads.push_back(thread);
				}
			}
		}
		for (auto iter : threads)
		{
			iter->join();
			delete iter;
		}
	
		closesocket(newConnection);
		WSACleanup();
		return 0;

	}
}
