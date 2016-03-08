#pragma once

#include "Tools/socket.h"


class AcceptedSocket : public Socket
{
	friend class ServerSocket;

private:
	AcceptedSocket();

private:
	sockaddr_in _address;
};
