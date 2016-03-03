#include <iostream>
#include <string>
#include <sstream>
#include <time.h>
#include <inttypes.h>


namespace Login
{
	void setNostalePath(std::string path);
	void setMD5(std::string nostaleX, std::string nostale);

	uint32_t seedRandom(uint32_t p0);
	std::string generateRandom1(uint32_t p0);
	std::string encryptPass(std::string& password);
	std::string login(std::string username, std::string password);

	uint32_t decimal_str2hex(std::string id);
	uint8_t encrypt_number(uint32_t id);
	uint8_t encrypt_key(uint32_t id);

	std::string encrytLogin(char* packet, int len);
	std::string decryptLogin(char* packet, int len);
	std::string encrytAnswer(char* packet, int len);
	std::string decrytAnswer(char* packet, int len);

	std::string encrytLogin(std::string& packet);
	std::string decryptLogin(std::string& packet);
	std::string encrytAnswer(std::string& packet);
	std::string decrytAnswer(std::string& packet);
}
