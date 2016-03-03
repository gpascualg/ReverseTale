#include "Game/packet.h"
#include "Game/game.h
"
namespace Game
{
	Packet::Packet(std::string&& packet):
		Packet(),
		_packet(packet)
	{}

	Packet::Packet(int size):
		Packet()
	{
		_packet.reserve(size);
	}

	Packet::Packet():
		_isCommitted(false), _isFinished(false)
	{}

	Packet::~Packet()
	{}

	void Packet::commit()
	{
		if (!_isCommitted)
		{
			_isCommitted = true;
			commit_imp();
		}
	}

	void Packet::finish()
	{
		assert(!_isFinished);
		commit();
		_isFinished = true;
		finish_imp();
	}


	ClientPacket::ClientPacket(Session* session, std::string&& packet):
		Packet(session, packet)
	{}

	ClientPacket::ClientPacket(Session* session, int size):
		Packet(session, size)
	{}

	ClientPacket::ClientPacket(Session* session):
		Packet(session)
	{}

	ClientPacket::~ClientPacket()
	{}

	void ClientPacket::commit_imp()
	{
		_packet = _session->
	}
}