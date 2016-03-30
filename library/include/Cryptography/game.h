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
		namespace Game
		{
			class Encrypter : public Crypto::Base::Encrypter
			{
			public:
				void commit(NString& packet) override;
				void finish(NString& packet, Utils::Game::Session* session = nullptr) override;

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
				std::vector<NString> parse(NString& packet, Utils::Game::Session* session) override;

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
		namespace Game
		{
			class Encrypter : public Crypto::Base::Encrypter
			{
			public:
				void commit(NString& packet) override;
				void finish(NString& packet, Utils::Game::Session* session) override;

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
				std::vector<NString> parse(NString& packet, Utils::Game::Session* session = nullptr) override;

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
