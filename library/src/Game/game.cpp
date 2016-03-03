#include "Game/game.h"
#include "Login/login.h"

namespace Game
{
	std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
		std::stringstream ss(s);
		std::string item;
		while (std::getline(ss, item, delim)) {
			elems.push_back(item);
		}
		return elems;
	}


	std::vector<std::string> tokenize(const std::string &s, char delim) {
		std::vector<std::string> elems;
		split(s, delim, elems);
		return elems;
	}

	Session::Session(std::string id)
	{
		setID(id);
	}

	Session::Session()
	{
		reset();
	}

	void Session::reset()
	{
		_id = "";
		_idHex = -1;
		_key = -1;
		_number = -1;
	}

	void Session::setID(std::string id)
	{
		_id = id;
		_idHex = Login::decimal_str2hex(id);
		_number = Login::encrypt_number(_idHex);
		_key = Login::encrypt_key(_idHex);

		printf("\n---- SET: %X / %.2X _ %.2X\n", _idHex, _number, _key);
	}

	std::string Session::encryptPacket_Phase1(char* packet, int len)
	{
		std::string phase1;
		std::string mask(len, '0');

		for (int i = 0; i < len; ++i)
		{
			char ch = packet[i];
			if (!(ch -= 0x20) || (ch += 0xF1) || (ch -= 0xB) || !(ch -= 0xC5))
			{
				mask[i] = '1';
			}
		}

		// len =					// EBP - 0C 
		int currentCounter = 0;		// EBP - 10
		int lastCounter = 0;		// EBP - 1C
		bool pair;

		while (currentCounter < len)
		{
			lastCounter = currentCounter;

			for (; currentCounter < len && mask[currentCounter] == '0'; ++currentCounter)
			{}

			if (currentCounter > lastCounter)
			{
				int currentLen = (currentCounter - lastCounter);
				while (currentLen > 0)
				{
					if (currentLen > 0x7E)
					{
						currentLen = 0x7E;
					}
					
					phase1 += currentLen;
				}
			}
			else
			{
				if (currentCounter < len)
				{
					lastCounter = currentCounter;
					pair = true;

					for (; currentCounter < len && mask[currentCounter] == '1'; ++currentCounter)
					{}

					if (currentCounter > lastCounter)
					{
						int currentLen = (currentCounter - lastCounter);
						if (currentLen > 0x7E)
						{
							currentLen = 0x7E;
						}
						else
						{
							phase1 += currentLen | 0x80;
						}

						while (currentLen > 0)
						{
							char ch = packet[lastCounter];
							++lastCounter;

							/*
							if (ch < ch + 0xD0 - 0xA) // Carry flag
							{
								ch += 0x4 - 0x30;
							}
							*/
							switch (ch)
							{
								case 0x20:
									ch = 0x1; break;
								case 0x2D:
									ch = 0x2; break;
								case 0x2E:
									ch = 0x3; break;
								case 0xFF:
									ch = 0xE; break;
								default:
									ch -= 0x2C; break;
							}

							if (pair)
							{
								phase1 += ch << 4;
							}
							else
							{
								phase1.back() |= ch;
							}

							pair = !pair;
							--currentLen;
						}
					}
				}
			}
		}

		phase1 += (uint8_t)0xFF;
		return phase1;
	}

	std::string Session::encryptPacket_Phase2(std::string& packet)
	{
		int i = 0;
		uint8_t chr = 0;
		int len = packet.length();

		switch (_number)
		{
			case 0:
				for (int i = 0; i < len; ++i)
				{
					chr = (uint8_t)packet[i];
					chr += (0x40 + _key);
					packet[i] = chr;
				}
				break;

			case 1:
				for (int i = 0; i < len; ++i)
				{
					chr = (uint8_t)packet[i];
					chr -= (0x40 + _key);
					packet[i] = chr;
				}
				break;

			case 2:
				for (int i = 0; i < len; ++i)
				{
					chr = (uint8_t)packet[i] ^ 0xC3;
					chr += (0x40 + _key);
					packet[i] = chr;
				}
				break;

			case 3:
				for (int i = 0; i < len; ++i)
				{
					chr = (uint8_t)packet[i] ^ 0xC3;
					chr -= (0x40 + _key);
					packet[i] = chr;
				}
				break;
				
			default:
				for (int i = 0; i < len; ++i)
				{
					packet[i] = (uint8_t)packet[i] + 0xF;
				}
				break;
		}

		return packet;
	}

	std::string Session::encryptPacket(char* packet, int len)
	{
		return encryptPacket_Phase2(encryptPacket_Phase1(packet, len));
	}

	std::string Session::decryptPacket_Phase1(char* packet, int len)
	{
		int i = 0;
		uint8_t chr = 0;
		std::string decrypted(packet, len);

		switch (_number)
		{
			case 0:
				for (int i = 0; i < len; ++i)
				{
					chr = (uint8_t)decrypted[i];
					chr -= (0x40 + _key);
					decrypted[i] = chr;
				}
				break;

			case 1:
				for (int i = 0; i < len; ++i)
				{
					chr = (uint8_t)decrypted[i];
					chr += (0x40 + _key);
					decrypted[i] = chr;
				}
				break;

			case 2:
				for (int i = 0; i < len; ++i)
				{
					chr = (uint8_t)decrypted[i];
					chr -= (0x40 + _key);
					decrypted[i] = chr ^ 0xC3;
				}
				break;

			case 3:
				for (int i = 0; i < len; ++i)
				{
					chr = (uint8_t)decrypted[i];
					chr += (0x40 + _key);
					decrypted[i] = chr ^ 0xC3;
				}
				break;

			default:
				for (int i = 0; i < len; ++i)
				{
					decrypted[i] -= 0xF;
				}
				break;
		}

		std::vector<std::string> temp = tokenize(decrypted, (uint8_t)0xFF);
		std::string save = "";

		std::cout << "PARTS: " << temp.size() << std::endl;

		for (std::size_t i = 0; i < temp.size(); i++)
		{
			save += decryptPacket_Phase2(temp[i]);
			save += (uint8_t)0xFF;
		}

		std::cout << "END DECRYPT" << std::endl;
		return save;
	}

	std::string Session::decryptPacket_Phase2(std::string& packet)
	{
		static uint8_t table[] = {
			0x00, 0x20, 0x2D, 0x2E, 0x30, 0x31, 0x32, 0x33,
			0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x0A, 0x00
		};

		uint32_t packetLen = (uint32_t)packet.length();
		std::string decrypted;

		std::cout << "PHASE 2 :: " << packetLen << "(";

		for (uint32_t counter = 0; counter < packetLen - 1;)
		{
			uint8_t chr = (uint8_t)packet[counter];
			++counter;
			printf("%d[%.2X], ", counter - 1, chr);

			if (chr == 0xFF)
			{
				// TODO: Add as single packet
				continue;
			}

			uint8_t localMax = chr & 0x7F;
			if (chr & 0x80)
			{
				while (localMax)
				{
					if (counter <= packetLen)
					{
						uint8_t high = packet[counter];
						uint8_t low = packet[counter];
						++counter;

						high &= 0xF0;
						high >>= 4;
						if (high > 0x0 && high < 0xF)
						{
							decrypted += table[high];
							--localMax;
						}

						low &= 0x0F;
						if (low > 0x0 && low < 0xF)
						{
							decrypted += table[low];
							--localMax;
						}
					}
					else
					{
						--localMax;
					}
				}
			}
			else
			{
				while (localMax)
				{
					if (counter < packetLen)
					{
						decrypted += packet[counter] ^ 0xFF;
						++counter;
					}
					--localMax;
				}
			}
		}

		std::cout << "...)" << std::endl;

		return decrypted;
	}

	std::string Session::decryptPacket(char* packet, int len)
	{
		return decryptPacket_Phase1(packet, len);
	}

	std::string Session::decryptRecv(char* packet, int len)
	{
		std::string decAcc;
		std::string decrypted(packet, len);
		auto parts = tokenize(decrypted, (uint8_t)0xFF);

		for (std::size_t i = 0; i < parts.size(); ++i)
		{
			decAcc += decryptPacket_Phase2(parts[i]);
		}
		return decAcc;
	}
}
