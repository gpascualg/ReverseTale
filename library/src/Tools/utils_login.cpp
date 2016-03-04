#include "Tools/md5.h"
#include "Tools/utils.h"
#include <cmath>


namespace Utils
{
	extern std::string nostalePath;
	extern std::string md5_nostaleX;
	extern std::string md5_nostale;
	extern bool md5IsSet;

	// TODO: Should be optimized
	std::string hex2hex_str(uint8_t w)
	{
		static char hexString[3];
		sprintf(hexString, "%.2X", w);
		hexString[2] = '\0';
		return std::string(hexString);
	}

	// TODO: Should be optimized
	std::string hex2hex_str(uint16_t w)
	{
		static char hexString[5];
		sprintf(hexString, "%.4X", w);
		hexString[4] = '\0';
		return std::string(hexString);
	}

	// TODO: Should be optimized
	std::string hex2hex_str(uint32_t w)
	{
		static char hexString[9];
		sprintf(hexString, "%.8X", w);
		hexString[8] = '\0';
		return std::string(hexString);
	}

	namespace Login
	{
		uint8_t password_table[] = {
			0x2E, 0x2A, 0x17, 0x4F, 0x20, 0x24, 0x47, 0x11,
			0x5B, 0x37, 0x53, 0x43, 0x15, 0x34, 0x45, 0x25,
			0x4B, 0x1D, 0x2F, 0x58, 0x2B, 0x32, 0x63
		};

		std::string decryptPass(std::string& hash)
		{
			int i = seedRandom(0x17);
			std::string password;
			password.reserve(hash.length() * 4 + 3); // 4bytes per letter + 3 random

			if (hash.length() % 2)
			{
				hash = hash.substr(2);
			}
			else
			{
				hash = hash.substr(3);
			}

			for (int n = 0; n < password.length(); n += 2)
			{
				uint8_t high = hash[n];
				uint8_t low = hash[n + 1];

				char chr = (high & 0xF) << 4 | (low & 0x0F);
				password += chr;
			}

			return password;
		}

		std::string encryptPass(std::string& password)
		{
			int i = seedRandom(0x17);
			const char* pass = password.c_str();
			std::string hash;
			hash.reserve(password.length() * 4 + 3); // 4bytes per letter + 3 random

			for (int n = 0; n < password.length(); ++n)
			{
				uint8_t actual = pass[n];
				uint8_t tabla = password_table[i];

				// tabla | actual
				uint16_t hash_i = ((tabla & 0xF0) | ((actual & 0xF0) >> 4)) << 8;
				hash_i |= ((tabla & 0x0F) << 4) | (actual & 0x0F);

				hash += hex2hex_str(hash_i);

				i = (++i) % 23;
			}

			i = seedRandom(0xA7);
			return hex2decimal_str(i) + hash;
		}

		std::string makePacket(std::string username, std::string password)
		{
			MD5 md5;

			if (!md5IsSet)
			{
				md5_nostaleX = md5.digestFile((nostalePath + "\\NostaleX.dat").c_str());
				md5_nostale = md5.digestFile((nostalePath + "\\Nostale.dat").c_str());
				md5IsSet = true;
			}

			std::string md5_hash = md5_nostaleX + md5_nostale + username;

			std::string packet("NoS0575 ");
			packet.reserve(8 + username.length() + 1 + password.length() * 4 + 4 + 9 + 1 + 13 + 32 + 1); // Pre-allocate size
			packet += hex2decimal_str(seedRandom(0x989680) + 0x86111);				// 8 bytes
			packet += std::string(" ") + username;									// username.length() + 1
			packet += std::string(" ") + encryptPass(password);						// password.length() * 4 + 3 + 1
			packet += std::string(" ") + hex2hex_str(seedRandom(0x989680));		// 8 bytes + 1
			packet += 0x0B;															// 1 byte
			packet += "0.9.3.3055 0 ";												// 13 bytes
			packet += md5.digestString(md5_hash);									// 32 bytes
			packet += 0x0A;															// 1 byte

			return packet;
		}
	}
}
