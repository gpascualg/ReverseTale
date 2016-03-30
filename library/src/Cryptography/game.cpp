#include "Cryptography/game.h"
#include "Tools/utils.h"


namespace Crypto
{
	namespace Base
	{
		void Phase1(NString& packet)
		{
			std::size_t len = packet.length();
			NString phase1;
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
								phase1 << (uint8_t)(currentLen - i);
							}
							else
							{
								phase1 << (uint8_t)0x7E;
								--sequences;
								++sequence_counter;
							}
						}

						phase1 << (uint8_t)(packet[lastCounter] ^ 0xFF);
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
					for (std::size_t i = 0; i < currentLen; i += 2)
					{
						if (i == (sequence_counter * 0x7E))
						{
							if (!sequences)
							{
								phase1 << (uint8_t)((currentLen - i) | 0x80);
							}
							else
							{
								phase1 << (uint8_t)(0x7E | 0x80);
								--sequences;
								++sequence_counter;
							}
						}

						char ch1 = packet[lastCounter];
						++lastCounter;

						char ch2 = packet[lastCounter];
						++lastCounter;

						switch ((uint8_t)ch1)
						{
							case 0x20:
								ch1 = 0x1; break;
							case 0x2D:
								ch1 = 0x2; break;
							case 0x2E:
								ch1 = 0x3; break;
							case 0xFF:
								ch1 = 0xE; break;
							default:
								ch1 -= 0x2C; break;
						}

						switch ((uint8_t)ch2)
						{
						case 0x20:
							ch2 = 0x1; break;
						case 0x2D:
							ch2 = 0x2; break;
						case 0x2E:
							ch2 = 0x3; break;
						case 0xFF:
							ch2 = 0xE; break;
						default:
							ch2 -= 0x2C; break;
						}

						phase1 << (uint8_t)((ch1 << 4) | ch2);
					}
				}
			}

			phase1 << (uint8_t)0xFF;
			packet = phase1;
		}

		std::vector<NString> Phase2(NString& packet)
		{
			static uint8_t table[] = {
				0x00, 0x20, 0x2D, 0x2E, 0x30, 0x31, 0x32, 0x33,
				0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x0A, 0x00
			};

			std::vector<NString> output;
			uint32_t packetLen = (uint32_t)packet.length();
			NString decrypted;

			for (uint32_t counter = 0; counter < packetLen - 1;)
			{
				uint8_t chr = (uint8_t)packet[counter];
				++counter;

				if (chr == 0xFF)
				{
					output.push_back(decrypted);
					packet = decrypted;
					decrypted = "";
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
								decrypted << (uint8_t)table[high];
								--localMax;
							}

							low &= 0x0F;
							if (low > 0x0 && low < 0xF)
							{
								decrypted << (uint8_t)table[low];
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
							decrypted << (uint8_t)(packet[counter] ^ 0xFF);
							++counter;
						}
						--localMax;
					}
				}
			}

			if (!decrypted.empty())
			{
				output.push_back(decrypted);
				packet = decrypted;
			}

			return output;
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

			void Encrypter::commit(NString& packet)
			{
				packet << (uint8_t)0x0A;
			}

			void Encrypter::finish(NString& packet, Utils::Game::Session* session)
			{
				Crypto::Base::Phase1(packet);
			}

			Decrypter::Decrypter():
				Crypto::Base::Decrypter()
			{}

			std::vector<NString> Decrypter::parse(NString& packet, Utils::Game::Session* session)
			{
				int i = 0;
				uint8_t chr = 0;
				uint8_t key = session->key() + 0x40;
				std::size_t len = packet.length();

				std::vector<NString> output;
				NString decrypted;

				switch (session->number())
				{
					case 0:
						for (std::size_t i = 0; i < len; ++i)
						{
							chr = (uint8_t)packet[i];
							chr -= key;

							if (chr == 0xFF)
							{
								Crypto::Base::Phase2(decrypted);
								output.push_back(decrypted);
								decrypted = "";
							}
							else
							{
								decrypted << (uint8_t)chr;
							}
						}
						break;

					case 1:
						for (std::size_t i = 0; i < len; ++i)
						{
							chr = (uint8_t)packet[i];
							chr += key;

							if (chr == 0xFF)
							{
								Crypto::Base::Phase2(decrypted);
								output.push_back(decrypted);
								decrypted = "";
							}
							else
							{
								decrypted << (uint8_t)chr;
							}
						}
						break;

					case 2:
						for (std::size_t i = 0; i < len; ++i)
						{
							chr = (uint8_t)packet[i];
							chr -= key;
							chr ^= 0xC3;

							if (chr == 0xFF)
							{
								Crypto::Base::Phase2(decrypted);
								output.push_back(decrypted);
								decrypted = "";
							}
							else
							{
								decrypted << (uint8_t)chr;
							}
						}
						break;

					case 3:
						for (std::size_t i = 0; i < len; ++i)
						{
							chr = (uint8_t)packet[i];
							chr += key;
							chr ^= 0xC3;

							if (chr == 0xFF)
							{
								Crypto::Base::Phase2(decrypted);
								output.push_back(decrypted);
								decrypted = "";
							}
							else
							{
								decrypted << (uint8_t)chr;
							}
						}
						break;

					default:
						for (std::size_t i = 0; i < len; ++i)
						{
							chr = packet[i] - 0xF;

							if (chr == 0xFF)
							{
								Crypto::Base::Phase2(decrypted);
								output.push_back(decrypted);
								decrypted = "";
							}
							else
							{
								decrypted << (uint8_t)chr;
							}
						}
						break;
				}

				if (decrypted.length() > 0)
				{
					output.push_back(decrypted);
				}

				packet = output.back();
				return output;
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

			void Encrypter::commit(NString& packet)
			{
				Crypto::Base::Phase1(packet);
			}

			void Encrypter::finish(NString& packet, Utils::Game::Session* session)
			{
				int i = 0;
				uint8_t chr = 0;
				std::size_t len = packet.length();
				uint8_t key = session->key() + 0x40;

				NString decrypted;

				switch (session->number())
				{
					case 0:
						for (std::size_t i = 0; i < len; ++i)
						{
							chr = (uint8_t)packet[i];
							chr += key;
							decrypted << (uint8_t)chr;
						}
						break;

					case 1:
						for (std::size_t i = 0; i < len; ++i)
						{
							chr = (uint8_t)packet[i];
							chr -= key;
							decrypted << (uint8_t)chr;
						}
						break;

					case 2:
						for (std::size_t i = 0; i < len; ++i)
						{
							chr = (uint8_t)packet[i] ^ 0xC3;
							chr += key;
							decrypted << (uint8_t)chr;
						}
						break;

					case 3:
						for (std::size_t i = 0; i < len; ++i)
						{
							chr = (uint8_t)packet[i] ^ 0xC3;
							chr -= key;
							decrypted << (uint8_t)chr;
						}
						break;
						
					default:
						for (std::size_t i = 0; i < len; ++i)
						{
							decrypted << (uint8_t)(packet[i] + 0xF);
						}
						break;
				}

				packet = decrypted;
			}

			Decrypter::Decrypter():
				Crypto::Base::Decrypter()
			{}

			std::vector<NString> Decrypter::parse(NString& packet, Utils::Game::Session* session)
			{
				std::vector<NString> output;

				for (int i = 0; i < packet.tokens(0xFF).length(); ++i)
				{
					NString part = packet.tokens()[i];

					Crypto::Base::Phase2(part);
					output.push_back(part);
				}

				packet = output.back();
				return output;
			}
		}
	}
}
