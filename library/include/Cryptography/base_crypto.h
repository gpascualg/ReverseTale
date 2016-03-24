#pragma once

#include <string>
#include <vector>
#include <Tools/nstring.h>

namespace Utils
{
	namespace Game
	{
		class Session;
	}
}

namespace Crypto
{
	namespace Base
	{
		class Encrypter
		{
		public:
			virtual void commit(NString& packet) {};
			virtual void finish(NString& packet, Utils::Game::Session* session = nullptr) = 0;

		protected:
			Encrypter();
		};

		class Decrypter
		{
		public:
			virtual std::vector<NString> parse(NString& packet, Utils::Game::Session* session = nullptr) = 0;

		protected:
			Decrypter();
		};
	}
}
