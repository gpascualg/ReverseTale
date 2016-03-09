#pragma once

#include "Tools/socket.h"


template <typename DataType>
class AcceptedSocket : public Socket
{
	template <typename T>
	friend class ServerSocket;

public:
	void setUserData(DataType data)
	{
		_userData = data;
	}

	DataType data()
	{
		return _userData;
	}


private:
	AcceptedSocket()
	{}


private:
	sockaddr_in _address;
	DataType _userData;
};


