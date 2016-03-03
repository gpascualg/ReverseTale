#include "Tools/md5.h"
#include "Login/login.h"
#include <cmath>

std::string hex2str(uint8_t w)
{
	static char hexString[3];
	sprintf(hexString, "%.2X", w);
	hexString[2] = '\0';
	return std::string(hexString);
}

std::string hex2str(uint16_t w)
{
	static char hexString[5];
	sprintf(hexString, "%.4X", w);
	hexString[4] = '\0';
	return std::string(hexString);
}

std::string hex2str(uint32_t w)
{
	static char hexString[9];
	sprintf(hexString, "%.8X", w);
	hexString[8] = '\0';
	return std::string(hexString);
}

namespace Login
{
	uint32_t _anterior = 0;
	uint32_t _operando2 = 0x8088405;
	uint32_t seedRandom(uint32_t p0)
	{
		uint32_t resultado = (uint32_t)((int)_anterior * (int)_operando2);
		++resultado;
		_anterior = resultado;
		uint64_t mult = (uint64_t)p0 * (uint64_t)resultado;
		return (uint32_t)(mult >> 32);
	}

	std::string generateRandom1(uint32_t p0)
	{
		const int len = std::log10(p0) + 1;
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

	uint8_t password_table[] = {
		0x2E, 0x2A, 0x17, 0x4F, 0x20, 0x24, 0x47, 0x11,
		0x5B, 0x37, 0x53, 0x43, 0x15, 0x34, 0x45, 0x25,
		0x4B, 0x1D, 0x2F, 0x58, 0x2B, 0x32, 0x63
	};


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

			hash += hex2str(hash_i);

			i = (++i) % 23;
		}

		i = seedRandom(0xA7);
		return generateRandom1(i) + hash;
	}

	std::string login(std::string username, std::string password)
	{
		MD5 md5;

		std::string basePath("P:\\Program Files (x86)\\GameforgeLive\\Games\\ESP_spa\\NosTale");
		std::string md5_nostaleX = md5.digestFile((basePath + "\\NostaleX.dat").c_str());	// 3D78AC41C49B735EFEE2008E2E0F1ED6
		std::string md5_nostale = md5.digestFile((basePath + "\\Nostale.dat").c_str());		// 9D07DAD6A3D2EFCF97630E8DF6FC4724
		std::string md5_hash = md5_nostaleX + md5_nostale + username;

		std::string packet("NoS0575 ");
		packet.reserve(8 + username.length() + 1 + password.length() * 4 + 4 + 9 + 1 + 13 + 32 + 1); // Pre-allocate size
		packet += generateRandom1(seedRandom(0x989680) + 0x86111);				// 8 bytes
		packet += std::string(" ") + username;									// username.length() + 1
		packet += std::string(" ") + encryptPass(password);						// password.length() * 4 + 3 + 1
		packet += std::string(" ") + hex2str(seedRandom(0x989680));				// 8 bytes + 1
		packet += 0x0B;															// 1 byte
		packet += "0.9.3.3055 0 ";												// 13 bytes
		packet += md5.digestString(md5_hash);									// 32 bytes
		packet += 0x0A;															// 1 byte

		return packet;
	}

	std::string encrytLogin(char* packet, int len)
	{
		std::string decrypted(packet, len);

		for (int i = 0; i < len; ++i)
		{
			decrypted[i] = (decrypted[i] ^ 0xC3) + 0x0F;
		}

		return decrypted;
	}

	std::string decryptLogin(char* packet, int len)
	{
		std::string decrypted(packet, len);

		for (int i = 0; i < len; ++i)
		{
			decrypted[i] = (decrypted[i] - 0x0F) ^ 0xC3;
		}

		return decrypted;
	}

	std::string encrytAnswer(char* packet, int len)
	{
		std::string decrypted(packet, len);

		for (int i = 0; i < len; ++i)
		{
			decrypted[i] = decrypted[i] + 0x0F;
		}

		return decrypted;
	}

	std::string decrytAnswer(char* packet, int len)
	{
		std::string decrypted(packet, len);

		for (int i = 0; i < len; ++i)
		{
			decrypted[i] = decrypted[i] - 0x0F;
		}

		return decrypted;
	}

	std::string encrytLogin(std::string& packet)
	{
		for (int i = 0; i < packet.length(); ++i)
		{
			packet[i] = (packet[i] ^ 0xC3) + 0x0F;
		}

		return packet;
	}

	std::string decryptLogin(std::string& packet)
	{
		for (int i = 0; i < packet.length(); ++i)
		{
			packet[i] = (packet[i] - 0x0F) ^ 0xC3;
		}

		return packet;
	}

	std::string encrytAnswer(std::string& packet)
	{
		for (int i = 0; i < packet.length(); ++i)
		{
			packet[i] = packet[i] + 0x0F;
		}

		return packet;
	}

	std::string decrytAnswer(std::string& packet)
	{
		for (int i = 0; i < packet.length(); ++i)
		{
			packet[i] = packet[i] - 0x0F;
		}

		return packet;
	}
}
