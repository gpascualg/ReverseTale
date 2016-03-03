#include "Cryptography/game.h"
#include "Game/game.h"


namespace Crypto
{
	namespace Base
	{
		void Phase1(std::string& packet)
		{
			std::size_t len = packet.length();
			std::string phase1;
			std::string mask(len, '0');

			for (std::size_t i = 0; i < len; ++i)
			{
				char ch = packet[i];
				if (ch != '#' && (!(ch -= 0x20) || (ch += 0xF1) < 0 || (ch -= 0xB) < 0 || !(ch -= 0xC5)))
				{
					mask[i] = '1';
				}
			}

			// len =					// EBP - 0C 
			std::size_t currentCounter = 0;		// EBP - 10
			std::size_t lastCounter = 0;		// EBP - 1C
			bool pair;
			std::size_t sequence_counter = 0;

			while (currentCounter < len)
			{
				lastCounter = currentCounter;

				for (; currentCounter < len && mask[currentCounter] == '0'; ++currentCounter)
				{}

				if (currentCounter)
				{
					std::size_t currentLen = (currentCounter - lastCounter);
					std::size_t sequences = (currentLen / 0x7E);
					for (std::size_t i = 0; i < currentLen; ++i)
					{
						if (i == (sequence_counter * 0x7E))
						{
							if (!sequences)
							{
								phase1 += (uint8_t)(currentLen - i);
							}
							else
							{
								phase1 += 0x7E;
								--sequences;
								++sequence_counter;
							}
						}

						phase1 += packet[lastCounter] ^ 0xFF;
						++lastCounter;
					}
				}

				if (currentCounter >= len)
				{
					break;
				}

				lastCounter = currentCounter;
				pair = true;

				for (; currentCounter < len && mask[currentCounter] == '1'; ++currentCounter)
				{}

				if (currentCounter)
				{
					std::size_t currentLen = (currentCounter - lastCounter);
					std::size_t sequences = (currentLen / 0x7E);
					for (std::size_t i = 0; i < currentLen; ++i)
					{
						if (i == (sequence_counter * 0x7E))
						{
							if (!sequences)
							{
								phase1 += (uint8_t)(currentLen - i) | 0x80;
							}
							else
							{
								phase1 += (uint8_t)(0x7E | 0x80);
								--sequences;
								++sequence_counter;
							}
						}

						char ch = packet[lastCounter];
						++lastCounter;

						switch ((uint8_t)ch)
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
					}
				}
			}

			phase1 += (uint8_t)0xFF;
			packet.assign(phase1);
		}

		std::string Phase2(std::string& packet)
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
	}

	namespace Server
	{
		namespace Game
		{
			Encrypter* Encrypter::_instance = nullptr;
			Decrypter* Decrypter::_instance = nullptr;

			Encrypter::Encrypter():
				Crypto::Base::Encrypter()
			{}

			void Encrypter::finish(std::string& packet, ::Game::Session* session)
			{
				Crypto::Base::Phase1(packet);
			}

			Decrypter::Decrypter():
				Crypto::Base::Decrypter()
			{}

			void Decrypter::parse(std::string& packet, ::Game::Session* session)
			{
				int i = 0;
				uint8_t chr = 0;
				uint8_t key = session->key() + 0x40;
				std::size_t len = packet.length();

				switch (session->number())
				{
					case 0:
						for (std::size_t i = 0; i < len; ++i)
						{
							chr = (uint8_t)packet[i];
							chr -= key;
							packet[i] = chr;
						}
						break;

					case 1:
						for (std::size_t i = 0; i < len; ++i)
						{
							chr = (uint8_t)packet[i];
							chr += key;
							packet[i] = chr;
						}
						break;

					case 2:
						for (std::size_t i = 0; i < len; ++i)
						{
							chr = (uint8_t)packet[i];
							chr -= key;
							packet[i] = chr ^ 0xC3;
						}
						break;

					case 3:
						for (std::size_t i = 0; i < len; ++i)
						{
							chr = (uint8_t)packet[i];
							chr += key;
							packet[i] = chr ^ 0xC3;
						}
						break;

					default:
						for (std::size_t i = 0; i < len; ++i)
						{
							packet[i] -= 0xF;
						}
						break;
				}

				std::vector<std::string> temp = ::Game::tokenize(packet, (uint8_t)0xFF);
				std::string save = "";

				for (std::size_t i = 0; i < temp.size(); i++)
				{
					save += Crypto::Base::Phase2(temp[i]);
					save += (uint8_t)0xFF;
				}

				packet.assign(save);
			}
		}
	}

	namespace Client
	{
		namespace Game
		{
			Encrypter* Encrypter::_instance = nullptr;
			Decrypter* Decrypter::_instance = nullptr;

			Encrypter::Encrypter():
				Crypto::Base::Encrypter()
			{}

			void Encrypter::commit(std::string& packet)
			{
				Crypto::Base::Phase1(packet);
			}

			void Encrypter::finish(std::string& packet, ::Game::Session* session)
			{
				int i = 0;
				uint8_t chr = 0;
				std::size_t len = packet.length();
				uint8_t key = session->key() + 0x40;

				switch (session->number())
				{
					case 0:
						for (std::size_t i = 0; i < len; ++i)
						{
							chr = (uint8_t)packet[i];
							chr += key;
							packet[i] = chr;
						}
						break;

					case 1:
						for (std::size_t i = 0; i < len; ++i)
						{
							chr = (uint8_t)packet[i];
							chr -= key;
							packet[i] = chr;
						}
						break;

					case 2:
						for (std::size_t i = 0; i < len; ++i)
						{
							chr = (uint8_t)packet[i] ^ 0xC3;
							chr += key;
							packet[i] = chr;
						}
						break;

					case 3:
						for (std::size_t i = 0; i < len; ++i)
						{
							chr = (uint8_t)packet[i] ^ 0xC3;
							chr -= key;
							packet[i] = chr;
						}
						break;
						
					default:
						for (std::size_t i = 0; i < len; ++i)
						{
							packet[i] = (uint8_t)packet[i] + 0xF;
						}
						break;
				}
			}

			Decrypter::Decrypter():
				Crypto::Base::Decrypter()
			{}

			void Decrypter::parse(std::string& packet, ::Game::Session* session)
			{
				std::string decAcc;
				auto parts = ::Game::tokenize(packet, (uint8_t)0xFF);

				for (std::size_t i = 0; i < parts.size(); ++i)
				{
					decAcc += Crypto::Base::Phase2(parts[i]);
				}
				
				packet.assign(decAcc);
			}
		}
	}
}
