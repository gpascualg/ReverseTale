#include "Tools/md5.h"
#include "Login/login.h"


std::string hex2str(BYTE w)
{
	char hexString[3];
	sprintf(hexString, "%.2X", w);
	hexString[2] = '\0';
	return std::string(hexString);
}


std::string hex2str(WORD w)
{
	char hexString[5];
	sprintf(hexString, "%.4X", w);
	hexString[4] = '\0';
	return std::string(hexString);
}

std::string hex2str(DWORD w)
{
	char hexString[9];
	sprintf(hexString, "%.8X", w);
	hexString[8] = '\0';
	return std::string(hexString);
}

namespace Login
{

	DWORD _anterior = 0;
	DWORD _operando2 = 0x8088405;
	DWORD seedRandom(DWORD p0)
	{
		DWORD resultado = (DWORD)((int)_anterior * (int)_operando2);
		++resultado;
		_anterior = resultado;
		uint64_t mult = (uint64_t)p0 * (uint64_t)resultado;
		return (DWORD)(mult >> 32);
	}


	std::string generateRandom1(DWORD p0)
	{
		const int len = 7;

		BYTE* buffer = new BYTE[len];
		memset(buffer, 0, len);
		BYTE* ESI = buffer + len;

		DWORD ECX = 0x0A;
		DWORD EDX = 0;

		while (p0 != 0)
		{
			DWORD EAX = p0 / ECX;
			EDX = p0 % ECX;
			p0 = EAX;
			--ESI;

			BYTE DL = (EDX & 0xFF);
			DL += 0x30;

			if (DL >= 0x3A)
			{
				DL += 0x07;
			}

			*ESI = DL;
		}

		std::string randomNumber;
		for (int i = 0; i < len; ++i)
		{
			if (buffer[i])
			{
				randomNumber += buffer[i];
			}
		}

		delete[] buffer;
		return randomNumber;
	}

	BYTE password_table[] = {
		0x2E, 0x2A, 0x17, 0x4F, 0x20, 0x24, 0x47, 0x11,
		0x5B, 0x37, 0x53, 0x43, 0x15, 0x34, 0x45, 0x25,
		0x4B, 0x1D, 0x2F, 0x58, 0x2B, 0x32, 0x63
	};


	std::string encryptPass(std::string& password)
	{
		int i = seedRandom(0x17);
		const char* pass = password.c_str();
		std::string hash;

		for (int n = 0; n < password.length(); ++n)
		{
			BYTE actual = pass[n];
			BYTE tabla = password_table[i];

			// tabla | actual
			WORD hash_i = ((tabla & 0xF0) | ((actual & 0xF0) >> 4)) << 8;
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

		std::string packet("NoS0575 ");
		packet += generateRandom1(seedRandom(0x989680) + 0x86111);
		packet += std::string(" ") + username;
		packet += std::string(" ") + encryptPass(password);
		packet += std::string(" ") + hex2str(seedRandom(0x989680));
		packet += 0x0B;
		packet += "0.9.3.3055 0 ";
		packet += md5.digestString(md5_nostaleX + md5_nostale + username);
		packet += 0x0A;

		return packet;
	}

	char* encrytLogin(char* packet, int len)
	{
		for (int i = 0; i < len; ++i)
		{
			packet[i] = (packet[i] ^ 0xC3) + 0x0F;
		}

		return packet;
	}

	char* decryptLogin(char* packet, int len)
	{
		for (int i = 0; i < len; ++i)
		{
			packet[i] = (packet[i] - 0x0F) ^ 0xC3;
		}

		return packet;
	}

	char* encrytAnswer(char* packet, int len)
	{
		for (int i = 0; i < len; ++i)
		{
			packet[i] = packet[i] + 0x0F;
		}

		return packet;
	}

	char* decrytAnswer(char* packet, int len)
	{
		for (int i = 0; i < len; ++i)
		{
			packet[i] = packet[i] - 0x0F;
		}

		return packet;
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