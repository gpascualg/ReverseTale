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
			
			if (hash.length() < 2)
			{
				return "";
			}

			if (hash.length() % 2 == 0)
			{
				hash = hash.substr(2);
			}
			else
			{
				while (hash.length() % 4 != 0)
				{
					hash = hash.substr(1);
				}
			}

			if (hash.empty())
			{
				return "";
			}

			for (int n = 0; n < hash.length() - 1; n += 4)
			{
				uint8_t high = hash[n + 1];
				uint8_t low = hash[n + 3];

				char tmp[] = { 0, 0, 0 };
				tmp[0] = high;
				tmp[1] = low;

				password += (uint8_t)strtol(tmp, NULL, 16);
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

		NString makePacket(std::string username, std::string password)
		{
			MD5 md5;

			if (!md5IsSet)
			{
				md5_nostaleX = md5.digestFile((nostalePath + "\\NostaleX.dat").c_str());
				md5_nostale = md5.digestFile((nostalePath + "\\Nostale.dat").c_str());
				md5IsSet = true;
			}

			std::string md5_hash = md5_nostaleX + md5_nostale + username;

			NString packet("NoS0575 ");
			//packet.reserve(8 + username.length() + 1 + password.length() * 4 + 4 + 9 + 1 + 13 + 32 + 1); // Pre-allocate size
			packet << (uint32_t)(seedRandom(0x989680) + 0x86111);
			packet << ' ' << username << ' ' << encryptPass(password);
			packet << ' ' << fmt::pad(fmt::hexu(seedRandom(0x989680)), 8, '0');
			packet << (uint8_t)0x0B << "0.9.3.3055 0 " << md5.digestString(md5_hash) << (uint8_t)0x0A;

			return packet;
		}
	}
}
