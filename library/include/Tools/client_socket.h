#pragma once

#include "Tools/socket.h"


class ClientSocket : public Socket
{
public:
	ClientSocket(int family, int type, int protocol);
	bool connect(std::string ip, int16_t port);
};
