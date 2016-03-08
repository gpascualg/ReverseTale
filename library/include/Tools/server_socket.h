#pragma once

#include "Tools/socket.h"


template <typename DataType>
class AcceptedSocket;

class ServerSocket : public Socket
{
public:
	ServerSocket(int family, int type, int protocol);
	bool serve(int16_t port, std::string ip = "127.0.0.1", int maxPending = 1024);

	template <typename DataType>
	AcceptedSocket<DataType>* accept();
};


template <typename DataType>
AcceptedSocket<DataType>* ServerSocket::accept()
{
	assert(_status == SocketStatus::SERVING);

	AcceptedSocket<DataType>* socket = new AcceptedSocket<DataType>();
	socklen_t len = sizeof(sockaddr_in);
	socket->_socket = ::accept(_socket, (struct sockaddr *)&socket->_address, &len);

	if (socket->_socket <= SOCKET_ERROR)
	{
		delete socket;
		return nullptr;
	}

	return socket;
}

