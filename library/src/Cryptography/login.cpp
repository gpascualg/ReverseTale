#include "Cryptography/login.h"
#include "Game/game.h"


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

			void Encrypter::finish(std::string& packet, Game::Session* session)
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

			void Decrypter::parse(std::string& packet, Game::Session* session)
			{
				std::size_t len = packet.length();
				for (int i = 0; i < len; ++i)
				{
					packet[i] = (packet[i] - 0x0F) ^ 0xC3;
				}
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

			void Encrypter::finish(std::string& packet, Game::Session* session)
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

			void Decrypter::parse(std::string& packet, Game::Session* session)
			{
				std::size_t len = packet.length();
				for (int i = 0; i < len; ++i)
				{
					packet[i] = packet[i] - 0x0F;
				}
			}
		}
	}
}
