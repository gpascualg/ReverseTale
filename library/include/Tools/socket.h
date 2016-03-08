#pragma once

#include <iostream>
#include <string>
#ifdef _WIN32
        #include <Windows.h>
#else
        #include <unistd.h>
        #include <stdio.h>
        #include <sys/types.h>
        #include <sys/socket.h>
        #include <netinet/in.h>
        #include <arpa/inet.h>
	#include <netinet/tcp.h>
        #include <netdb.h>
#endif


#ifdef ERROR
	#undef ERROR
#endif

#ifndef _WIN32
	#define closesocket ::close
	#define SOCKADDR struct sockaddr
#endif

#ifndef SOCKET_ERROR
	#define SOCKET_ERROR -1
#endif


enum class SocketStatus
{
	DISCONNECTED,
	CONNECTED,
	SERVING,
	CLOSED,
	ERROR
};

enum class SocketError
{
	NONE,
	INITIALIZATION_ERROR,
	INSTANTIATION_ERROR,
	CONNECT_ERROR,
	BIND_ERROR,
	LISTEN_ERROR,
	SEND_ERROR,
	RECV_ERROR,
	BROKEN_PIPE
};

class Socket
{
public:
	virtual ~Socket();

	bool setOption(int level, int option, const char* value, int valueLength);

	inline SocketStatus status() { return _status; }
	inline SocketError error() { return _error; }
	
	int send(const char* buffer, int len);
	int send(const std::string& buffer) { return send(buffer.c_str(), buffer.length()); }
	std::string recv();
	void close();

protected:
	Socket();
	Socket(int family, int type, int protocol);
	void setError(SocketError error);

protected:
	static int _initialized;
	int _socket;
	int _family;
	int _type;
	int _protocol;
	char* _buf;
	SocketStatus _status;
	SocketError _error;
};

