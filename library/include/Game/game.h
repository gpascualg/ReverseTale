#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <time.h>
#include <inttypes.h>

namespace Game
{
	std::vector<std::string> tokenize(const std::string &s, char delim = ' ');

	class Session
	{
	public:
		Session(std::string id);
		Session();

		void reset();
		void setID(std::string id);
		std::string encryptPacket(char* packet, int len);
		std::string decryptSession(char* packet, int len);
		std::string decryptPacket(char* packet, int len);

		#pragma deprecated(decryptSession)

		std::string decryptRecv(char* packet, int len);

		inline uint32_t id() { return _idHex; }

	private:
		std::string encryptPacket_Phase1(char* packet, int len);
		std::string encryptPacket_Phase2(std::string& packet);

		std::string decryptPacket_Phase1(char* packet, int len);
		std::string decryptPacket_Phase2(std::string& packet);

	private:
		std::string _id;
		uint32_t _idHex;
		uint8_t _number;
		uint8_t _key;
	};
};
