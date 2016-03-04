#pragma once

#include <string>
#include <vector>


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
			virtual void commit(std::string& packet) {};
			virtual void finish(std::string& packet, Utils::Game::Session* session = nullptr) = 0;

		protected:
			Encrypter();
		};

		class Decrypter
		{
		public:
			virtual std::vector<std::string> parse(std::string& packet, Utils::Game::Session* session = nullptr) = 0;

		protected:
			Decrypter();
		};
	}
}
