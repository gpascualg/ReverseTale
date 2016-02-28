#pragma once

#include <iostream>
#include <string>


#ifdef ERROR
	#undef ERROR
#endif

enum class SocketStatus
{
	DISCONNECTED,
	CONNECTED,
	ERROR
};

enum class SocketError
{
	NONE,
	INITIALIZATION_ERROR,
	INSTANTIATION_ERROR,
	CONNECT_ERROR,
	SEND_ERROR,
	RECV_ERROR,
	BROKEN_PIPE
};

class Socket
{
public:
	Socket(int family, int type, int protocol);
	virtual ~Socket();

	bool setOption(int level, int option, const char* value, int valueLength);
	bool connect(std::string ip, int16_t port);

	int send(const char* buffer, int len);
	int recv(char* buffer, int size);

private:
	void setError(SocketError error);

private:
	static int _initialized;
	int _socket;
	SocketStatus _status;
	SocketError _error;
};
