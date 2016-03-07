#pragma once

#include <string>
#include <cassert>
#include <list>
#include <vector>


namespace Crypto
{
	namespace Base
	{
		class Encrypter;
		class Decrypter;
	}
}

namespace Utils
{
	namespace Game
	{
		class Session;
	}
}

class Socket;

namespace Net
{
	class Session;
	class Packet;

	enum class PacketType
	{
		CLIENT_LOGIN,
		CLIENT_GAME,
		SERVER_LOGIN,
		SERVER_GAME
	};

	class Factory
	{
		friend class Packet;

	public:
		static Factory* get()
		{
			if (!_instance)
			{
				_instance = new Factory();
			}
			return _instance;
		}

		Packet* make(PacketType type, std::string data = "") { return make(type, nullptr, data); }
		Packet* make(PacketType type, Utils::Game::Session* session) { return make(type, session, ""); }
		Packet* make(PacketType type, Utils::Game::Session* session, std::string data);
		void recycle(Packet* packet);

	private:
		std::list<Packet*> _pool;
		static Factory* _instance;
	};

	static Factory* gFactory = Factory::get();


	class Packet
	{
	public:
		Packet(std::string& packet);
		Packet(int size);
		Packet();
		virtual ~Packet();

		void forceCommit() { _isCommitted = true; }
		void commit();
		void finish();
		void send(Socket* socket);

		std::vector<std::string> decrypt();

		void setType(PacketType type);
		void setData(std::string data);
		void setSession(Utils::Game::Session* session);
		void setCrypto(Crypto::Base::Encrypter* crypter, Crypto::Base::Decrypter* decrypter);

		Packet& operator<<(char* str);
		Packet& operator<<(Packet& packet);
		Packet& operator<<(std::string& str);
		Packet& operator<<(std::string&& str);

		inline const std::string& data() { return _packet; }
		inline PacketType type() { return _type; }
		inline Utils::Game::Session* session() { return _session; }

	protected:
		PacketType _type;
		std::string _packet;
		bool _isCommitted;
		bool _isFinished;

		Utils::Game::Session* _session;
		Crypto::Base::Encrypter* _crypter;
		Crypto::Base::Decrypter* _decrypter;
	};

	Packet* operator+(Packet& one, Packet& other);
}
