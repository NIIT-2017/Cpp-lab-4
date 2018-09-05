#include "../../include/Client/Client.h"
#include <zlib.h>
SOCKET Connection;
std::mutex mtx;


char * crypt(const char * information, unsigned long length)
{
	char * out = new char[length + 1];
	for (int i = 0; i < length; i++)
	{
		out[i] = information[i] ^ 20;
	}
	out[length] = '\0';
	return out;
}


void clientHandler()
{
	std::lock_guard<std::mutex> lock(mtx);
	unsigned long rec_msg_size, msg_size;
	int transmode;
	char * rec_msg;
	recv(Connection, (char*)&transmode, sizeof(int), NULL);
	if (transmode < 2)
	{
		recv(Connection, (char*)&rec_msg_size, sizeof(unsigned long), NULL);
		rec_msg = new char[rec_msg_size + 1]{ 0 };
	}
	else {
		recv(Connection, (char*)&rec_msg_size, sizeof(unsigned long), NULL);
		recv(Connection, (char*)&msg_size, sizeof(unsigned long), NULL);
		rec_msg = new char[rec_msg_size + 1]{ 0 };
	}

	int total = 0;
	int n = 0;
	while (rec_msg_size > total)
	{
		n = recv(Connection, reinterpret_cast<char*>(rec_msg + total), rec_msg_size - total, NULL);
		if (n == 1) { break; }
		total += n;
	}

	switch (transmode)
	{
	case 0:
	{
		std::cout << rec_msg << std::endl;
		break;
	}
	case 1:
	{
		char * decryptmsg = crypt(rec_msg, rec_msg_size);
		std::cout << decryptmsg << std::endl;
		delete[] decryptmsg;
		break;
	}
	case 2:
	{
		char * msg = new char[msg_size + 1]{ 0 };
		if (Z_OK == uncompress(reinterpret_cast<unsigned char*>(msg), &msg_size, reinterpret_cast<unsigned char*>(rec_msg), rec_msg_size))
		{
			std::cout << msg << std::endl;
			std::cout << "Compress ratio: " << rec_msg_size / (float)msg_size << std::endl;
		}
		delete[] msg;
		break;
	}
	case 3:
	{
		char * msg = new char[msg_size + 1]{ 0 };
		if (Z_OK == uncompress(reinterpret_cast<unsigned char*>(msg), &msg_size, reinterpret_cast<unsigned char*>(rec_msg), rec_msg_size))
		{
			char * decryptmsg = crypt(msg, msg_size);
			std::cout << decryptmsg << std::endl;
			std::cout << "Compress ratio: " << rec_msg_size / (float)msg_size << std::endl;
			delete[] decryptmsg;
		}
		delete[] msg;
		break;
	}
	default:
		break;
	}
	delete[] rec_msg;

}