#pragma once

#include <string>
#include <cassert>

namespace Game
{
	class Session;

	class Packet
	{
	public:
		void commit();
		void finish();

	protected:
		Packet(std::string&& packet);
		Packet(int size);
		Packet();
		virtual ~Packet();

		virtual void commit_imp();
		virtual void finish_imp();

		std::string _packet;
		bool _isCommited;
		bool _isFinished;
	};

	class ClientPacket : public Packet
	{
	public:
		ClientPacket(Session* session, std::string&& packet);
		ClientPacket(Session* session, int size);
		ClientPacket(Session* session);
		virtual ~ClientPacket();

	private:
		Session* _session;		
	};
}