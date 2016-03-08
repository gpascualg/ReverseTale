#pragma once

#include "Tools/socket.h"


template <typename DataType>
class AcceptedSocket : public Socket
{
	friend class ServerSocket;
public:
	inline void setUserData(DataType data);
	inline DataType data();

private:
	AcceptedSocket();

private:
	sockaddr_in _address;
	DataType* _userData;
};

void AcceptedSocket::setUserData(DataType data)
{
	_userData = data;
}

DataType AcceptedSocket::data()
{
	return _userData;
}
