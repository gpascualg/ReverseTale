#pragma once

#include "Cryptography/base_crypto.h"


namespace Utils
{
	namespace Game
	{
		class Session;
	}
}

namespace Crypto
{
	namespace Server
	{
		namespace Login
		{
			class Encrypter : public Crypto::Base::Encrypter
			{
			public:
				void finish(std::string& packet, Utils::Game::Session* session = nullptr);

				static Crypto::Base::Encrypter* get()
				{
					if (!_instance)
					{
						_instance = new Encrypter();
					}
					return _instance;
				}

			private:
				Encrypter();
				static Encrypter* _instance;
			};

			class Decrypter : public Crypto::Base::Decrypter
			{
			public:
				std::vector<std::string> parse(std::string& packet, Utils::Game::Session* session = nullptr);

				static Crypto::Base::Decrypter* get()
				{
					if (!_instance)
					{
						_instance = new Decrypter();
					}
					return _instance;
				}

			private:
				Decrypter();
				static Decrypter* _instance;
			};
		}
	}

	namespace Client
	{
		namespace Login
		{
			class Encrypter : public Crypto::Base::Encrypter
			{
			public:
				void finish(std::string& packet, Utils::Game::Session* session = nullptr);

				static Crypto::Base::Encrypter* get()
				{
					if (!_instance)
					{
						_instance = new Encrypter();
					}
					return _instance;
				}

			private:
				Encrypter();
				static Encrypter* _instance;
			};


			class Decrypter : public Crypto::Base::Decrypter
			{
			public:
				std::vector<std::string> parse(std::string& packet, Utils::Game::Session* session = nullptr);

				static Crypto::Base::Decrypter* get()
				{
					if (!_instance)
					{
						_instance = new Decrypter();
					}
					return _instance;
				}

			private:
				Decrypter();
				static Decrypter* _instance;
			};
		}
	}
}
