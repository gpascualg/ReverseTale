#include "Cryptography/login.h"
#include "Tools/utils.h"


namespace Crypto
{
	namespace Server
	{
		namespace Login
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
				std::size_t len = packet.length();
				NString decrypted;

				for (std::size_t i = 0; i < len; ++i)
				{
					decrypted << (uint8_t)(packet[i] + 0x0F);
				}

				packet = decrypted;
			}

			Decrypter::Decrypter():
				Crypto::Base::Decrypter()
			{}

			std::vector<NString> Decrypter::parse(NString& packet, Utils::Game::Session* session)
			{
				std::size_t len = packet.length();
				NString decrypted;

				for (std::size_t i = 0; i < len; ++i)
				{
					decrypted << (uint8_t)((packet[i] - 0x0F) ^ 0xC3);
				}

				packet = decrypted;
				return std::vector<NString> { decrypted };
			}
		}
	}

	namespace Client
	{
		namespace Login
		{
			Encrypter* Encrypter::_instance = nullptr;
			Decrypter* Decrypter::_instance = nullptr;

			Encrypter::Encrypter():
				Crypto::Base::Encrypter()
			{}

			void Encrypter::finish(NString& packet, Utils::Game::Session* session)
			{
				std::size_t len = packet.length();
				NString decrypted;

				for (std::size_t i = 0; i < len; ++i)
				{
					decrypted << (uint8_t)((packet[i] ^ 0xC3) + 0x0F);
				}

				packet = decrypted;
			}

			Decrypter::Decrypter():
				Crypto::Base::Decrypter()
			{}

			std::vector<NString> Decrypter::parse(NString& packet, Utils::Game::Session* session)
			{
				std::size_t len = packet.length();
				NString decrypted;

				for (std::size_t i = 0; i < len; ++i)
				{
					decrypted << (uint8_t)(packet[i] - 0x0F);
				}

				packet = decrypted;
				return std::vector<NString> { decrypted };
			}
		}
	}
}
