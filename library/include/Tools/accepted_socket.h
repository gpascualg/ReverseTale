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

	virtual int send(std::string& packet)
	{
		assert(_status == SocketStatus::CONNECTED);

		_sendQueue.push(new std::string(packet));
		return -EWOULDBLOCK;
	}

	virtual void onRead(std::string packet) = 0;
	virtual void onWrite()
	{
		while (!_sendQueue.empty())
		{
			std::string* packet;
			if (_sendQueue.pop(packet))
			{
				Socket::send(*packet);
				delete packet;
			}
		}
	}

	void close()
	{
		_status = SocketStatus::CLOSING;

		if (_sendQueue.empty())
		{
			Socket::close();
		}
	}

private:
	sockaddr_in _address;

	boost::lockfree::queue<std::string*> _sendQueue;
};


