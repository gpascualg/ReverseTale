#include <iostream>
#include <string>
#include <sstream>
#include <time.h>
#include <inttypes.h>


namespace Login
{
	uint32_t seedRandom(uint32_t p0);
	std::string generateRandom1(uint32_t p0);
	std::string encryptPass(std::string& password);
	std::string login(std::string username, std::string password);

	char* encrytLogin(char* packet, int len);
	char* decryptLogin(char* packet, int len);
	char* encrytAnswer(char* packet, int len);
	char* decrytAnswer(char* packet, int len);

	std::string encrytLogin(std::string& packet);
	std::string decryptLogin(std::string& packet);
	std::string encrytAnswer(std::string& packet);
	std::string decrytAnswer(std::string& packet);
}
