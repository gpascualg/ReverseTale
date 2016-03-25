#pragma once

#include <string>

#include <asyncwork.h>
#include <Tools/utils.h>
#include <Tools/accepted_socket.h>


struct ClientWork;
template <typename T>
struct FutureWork;

class Client : public AcceptedSocket
{
private:
	bool workRouter(AbstractWork* work);
	bool handleConnect(ClientWork* work);
	bool handleUserCredentials(ClientWork* work);
	bool handlePasswordCredentials(ClientWork* work);
	bool sendConnectionResult(FutureWork<bool>* work);

public:
	Client();

	inline Utils::Game::Session* session() { return &_session; }
	void onRead(NString packet);
	void sendError(std::string&& error);

private:
	Utils::Game::Session _session;
	std::string _username;
	WorkType _currentWork;
};
