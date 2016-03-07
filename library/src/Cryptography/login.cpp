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

			void Encrypter::finish(std::string& packet, Utils::Game::Session* session)
			{
				std::size_t len = packet.length();
				for (int i = 0; i < len; ++i)
				{
					packet[i] = packet[i] + 0x0F;
				}
			}

			Decrypter::Decrypter():
				Crypto::Base::Decrypter()
			{}

			std::vector<std::string> Decrypter::parse(std::string& packet, Utils::Game::Session* session)
			{
				std::size_t len = packet.length();
				for (int i = 0; i < len; ++i)
				{
					packet[i] = (packet[i] - 0x0F) ^ 0xC3;
				}

				return std::vector<std::string> { packet };
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

			void Encrypter::finish(std::string& packet, Utils::Game::Session* session)
			{
				std::size_t len = packet.length();
				for (int i = 0; i < len; ++i)
				{
					packet[i] = (packet[i] ^ 0xC3) + 0x0F;
				}
			}

			Decrypter::Decrypter():
				Crypto::Base::Decrypter()
			{}

			std::vector<std::string> Decrypter::parse(std::string& packet, Utils::Game::Session* session)
			{
				std::size_t len = packet.length();
				for (int i = 0; i < len; ++i)
				{
					packet[i] = packet[i] - 0x0F;
				}

				return std::vector<std::string> { packet };
			}
		}
	}
}
