#include "Tools/server_socket.h"
#include "Tools/accepted_socket.h"

#include <cassert>


ServerSocket::ServerSocket(int family, int type, int protocol) :
	Socket(family, type, protocol)
{}

bool ServerSocket::serve(int16_t port, std::string ip, int maxPending)
{
	assert(_status == SocketStatus::DISCONNECTED);

	sockaddr_in clientService;
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr(ip.c_str());
	clientService.sin_port = htons(port);

	int result = ::bind(_socket, (SOCKADDR*)&clientService, sizeof(clientService));
	if (result == SOCKET_ERROR)
	{
		setError(SocketError::BIND_ERROR);
		return false;
	}

	result = ::listen(_socket, 1024);
	if (result == SOCKET_ERROR)
	{
		setError(SocketError::LISTEN_ERROR);
		return false;
	}

	_status = SocketStatus::SERVING;
	return true;
}

AcceptedSocket* ServerSocket::accept(AcceptedSocket* socket)
{
	assert(_status == SocketStatus::SERVING);

	socklen_t len = sizeof(sockaddr_in);
	socket->_socket = ::accept(_socket, (struct sockaddr *)&socket->_address, &len);

	if (socket->_socket <= SOCKET_ERROR)
	{
		delete socket;
		return nullptr;
	}

	socket->_status = SocketStatus::CONNECTED;
	return socket;
}
