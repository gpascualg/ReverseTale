#pragma once

#include <Tools/nstring.h>

#include <iostream>
#include <string>
#include <sstream>
#include <time.h>
#include <inttypes.h>
#include <vector>

namespace Utils
{
	void setNostalePath(std::string path);
	void setMD5(std::string nostaleX, std::string nostale);

	uint32_t seedRandom(uint32_t p0);
	std::string hex2decimal_str(uint32_t p0);
	uint32_t decimal_str2hex(std::string id);
	std::vector<std::string> tokenize(const std::string &s, char delim = ' ');

	uint8_t encrypt_number(uint32_t id);
	uint8_t encrypt_key(uint32_t id);

	namespace Login
	{
		std::string decryptPass(std::string& password);
		std::string encryptPass(std::string& password);
		NString makePacket(std::string username, std::string password);
	}

	namespace Game
	{
		class Session
		{
		public:
			Session(std::string id);
			Session();

			void reset();
			void setID(std::string id);
			void setAlive(uint16_t alive);

			uint16_t alive();
			bool check_alive(std::string& alive);
			NString next_alive();

			inline uint32_t id() { return _idHex; }
			inline uint8_t number() { return _number; }
			inline uint8_t key() { return _key; }

		private:
			std::string _id;
			uint32_t _idHex;
			uint16_t _alive;
			uint8_t _number;
			uint8_t _key;
		};
	};

}
