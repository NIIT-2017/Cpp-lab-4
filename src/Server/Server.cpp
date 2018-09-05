#include "../../include/Server/Server.h"



int fill(ServerConfig & cnf, const char * str1, const char * str2, int count)
{
	switch (count)
	{
	case 0:
		if (!strcmp(str1, "ip"))
		{
			cnf.addr.sin_addr.s_addr = inet_addr(str2);
			return count;
		}
		break;
	case 1:
		if (!strcmp(str1, "port"))
		{
			cnf.addr.sin_port = htons(atoi(str2));
			return count;
		}
	case 2:
		if (!strcmp(str1, "transfer_mode"))
		{
			cnf.transmode = atoi(str2);
			return count;
		}
		break;
	default:
		return -1;
	}
}
bool fillConfig(ServerConfig & cnf, const std::string & path)
{
	std::ifstream fin;
	fin.open(path);
	if (!fin.is_open())
	{
		std::cout << "File is not open" << std::endl;
		exit(1);
	}
	std::string s1;
	char *str1, *str2;
	int k = 0;
	while (getline(fin, s1))
	{
		if (!s1.size()) continue;

		str1 = strtok((char*)s1.c_str(), "=");
		str2 = str1;
		str1 = strtok(NULL, "\n");

		if (str2 != nullptr)
		{
			if (fill(cnf, str2, str1, k) != k)
			{
				std::cout << "Read data error" << std::endl;
				exit(3);
			}
			k++;
		}
	}
	fin.close();
	if (k != 3)
	{
		std::cout << "Not enough data in the file" << std::endl;
		return false;
	}
	cnf.addr.sin_family = AF_INET;
	return true;
}

std::map<int, std::string> fillAforisms(const std::string & path)
{
	std::map<int, std::string> mapString;
	std::fstream fin;
	fin.open(path);
	if (!fin.is_open())
	{
		std::cout << "File is not open!" << std::endl;
		exit(1);
	}
	std::string tmp;
	int count = 0;
	while (std::getline(fin, tmp))
	{
		if (!tmp.size()) continue;

		std::string add = tmp;
		mapString[count++] = tmp;
	}
	return mapString;
}

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