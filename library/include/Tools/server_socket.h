#pragma once

#include "Tools/socket.h"

#include <cassert>

#ifdef _WIN32
	typedef int socklen_t;
#endif

class AcceptedSocket;

class ServerSocket : public Socket
{
public:
	ServerSocket(int family, int type, int protocol);
	bool serve(int16_t port, std::string ip = "127.0.0.1", int maxPending = 1024);
	AcceptedSocket* accept(AcceptedSocket* socket);
};


