#pragma once

#include <string>


namespace Game
{
	class Session;
}

namespace Crypto
{
	namespace Base
	{
		class Encrypter
		{
		public:
			virtual void commit(std::string& packet) {};
			virtual void finish(std::string& packet, Game::Session* session = nullptr) = 0;

		protected:
			Encrypter();
		};

		class Decrypter
		{
		public:
			virtual void parse(std::string& packet, Game::Session* session = nullptr) = 0;

		protected:
			Decrypter();
		};
	}
}
