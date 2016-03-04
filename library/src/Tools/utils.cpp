#include "Tools/md5.h"
#include "Tools/utils.h"
#include <cmath>


namespace Utils
{
	uint32_t lastRandom = 0;
	std::string nostalePath;
	std::string md5_nostaleX;
	std::string md5_nostale;
	bool md5IsSet = false;

	std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems)
	{
		std::stringstream ss(s);
		std::string item;
		while (std::getline(ss, item, delim))
		{
			elems.push_back(item);
		}
		return elems;
	}

	std::vector<std::string> tokenize(const std::string &s, char delim)
	{
		std::vector<std::string> elems;
		split(s, delim, elems);
		return elems;
	}

	uint32_t seedRandom(uint32_t p0)
	{
		uint32_t result = (uint32_t)((int)lastRandom * (int)0x8088405);
		++result;
		lastRandom = result;
		uint64_t mult = (uint64_t)p0 * (uint64_t)result;
		return (uint32_t)(mult >> 32);
	}

	std::string hex2decimal_str(uint32_t p0)
	{
		const int len = (int)std::log10(p0) + 1;
		std::string randomNumber(len, '\0');

		uint8_t ESI = len;
		uint32_t ECX = 0x0A;
		uint32_t EDX = 0;

		while (p0 != 0)
		{
			uint32_t EAX = p0 / ECX;
			EDX = p0 % ECX;
			p0 = EAX;
			--ESI;

			uint8_t DL = (EDX & 0xFF);
			DL += 0x30;

			if (DL >= 0x3A)
			{
				DL += 0x07;
			}

			randomNumber[ESI] = DL;
		}

		return randomNumber;
	}

	uint32_t decimal_str2hex(std::string id)
	{
		int i = 0;
		uint32_t result = 0;
		const char* cid = id.c_str();

		char chr = cid[i++];
		while (chr != 0)
		{
			chr -= 0x30;
			if (chr > 9 || result >= 0xCCCCCCC)
			{
				return result;
			}

			result += result << 2;
			result <<= 1;
			result += chr;
			chr = cid[i++];
		}

		return result;
	}

	void setNostalePath(std::string path)
	{
		nostalePath = path;
	}

	void setMD5(std::string nostaleX, std::string nostale)
	{
		md5IsSet = true;
		md5_nostaleX = nostaleX;
		md5_nostale = nostale;
	}

	uint8_t encrypt_number(uint32_t id)
	{
		id = id >> 6; // ID >>= 6;
		id = id & 0xFF;
		id &= 0x80000003;
		if (id >= 0x80000000)
		{
			--id;
			id |= 0xFFFFFFFC;
			++id;
		}

		return id;
	}

	uint8_t encrypt_key(uint32_t id)
	{
		return id & 0xFF;
	}
}
