#pragma once

#include <string>
#include <cassert>
#include <list>
#include <vector>
#include <Tools/nstring.h>


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

		Packet* make(PacketType type, NString data = NString()) { return make(type, nullptr, data); }
		Packet* make(PacketType type, Utils::Game::Session* session) { return make(type, session, NString()); }
		Packet* make(PacketType type, Utils::Game::Session* session, NString data);
		void recycle(Packet* packet);

	private:
		std::list<Packet*> _pool;
		static Factory* _instance;
	};

	static Factory* gFactory = Factory::get();


	class Packet
	{
	public:
		Packet(NString packet);
		Packet(int size);
		Packet();
		virtual ~Packet();

		void forceCommit() { _isCommitted = true; }
		void commit();
		void finish();
		void send(Socket* socket);

		std::vector<NString> decrypt();

		void setType(PacketType type);
		void setData(NString data);
		void setSession(Utils::Game::Session* session);
		void setCrypto(Crypto::Base::Encrypter* crypter, Crypto::Base::Decrypter* decrypter);

		template <typename T>
		inline Packet& operator<<(T val)
		{
			_packet << val;
			return *this;
		}

		Packet& operator<<(Packet& packet);
		Packet& operator<<(NString str);
		//Packet& operator<<(std::string&& str);

		inline NString data() { return _packet; }
		inline PacketType type() { return _type; }
		inline Utils::Game::Session* session() { return _session; }

	protected:
		PacketType _type;
		NString _packet;
		bool _isCommitted;
		bool _isFinished;

		Utils::Game::Session* _session;
		Crypto::Base::Encrypter* _crypter;
		Crypto::Base::Decrypter* _decrypter;
	};

	Packet* operator+(Packet& one, Packet& other);
}
