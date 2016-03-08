#include "Tools/client_socket.h"

#include <cassert>


ClientSocket::ClientSocket(int family, int type, int protocol):
	Socket(family, type, protocol)
{}

bool ClientSocket::connect(std::string ip, int16_t port)
{
	assert(_status == SocketStatus::DISCONNECTED);

	sockaddr_in clientService;
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr(ip.c_str());
	clientService.sin_port = htons(port);

	int result = ::connect(_socket, (SOCKADDR*)&clientService, sizeof(clientService));
	if (result == SOCKET_ERROR)
	{
		setError(SocketError::CONNECT_ERROR);
		return false;
	}

	_status = SocketStatus::CONNECTED;
	return true;
}
