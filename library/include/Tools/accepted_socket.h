#pragma once

#include "Tools/socket.h"

#include <boost/lockfree/queue.hpp>

class AcceptedSocket : public Socket
{
	friend class ServerSocket;

	template <typename T>
	friend class Reactor;

public:
	AcceptedSocket():
		_sendQueue(256)
	{}

	virtual int send(NString packet) override
	{
		assert(_status == SocketStatus::CONNECTED);

		_sendQueue.push(new NString((const NString&)packet));
		return -EWOULDBLOCK;
	}

	virtual void onRead(NString packet) = 0;
	virtual void onWrite()
	{
		while (!_sendQueue.empty())
		{
			NString* packet;
			if (_sendQueue.pop(packet))
			{
				Socket::send(*packet);
				delete packet;
			}
		}
	}

	void close() override
	{
		_status = SocketStatus::CLOSING;

		if (_sendQueue.empty())
		{
			Socket::close();
		}
	}

private:
	sockaddr_in _address;

	boost::lockfree::queue<NString*> _sendQueue;
};


