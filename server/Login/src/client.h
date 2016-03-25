#pragma once

#include <string>

#include <Tools/utils.h>
#include <Tools/accepted_socket.h>


struct ClientWork;
template <typename T>
struct FutureWork;

class Client : public AcceptedSocket
{
private:
	bool handleReadLogin(ClientWork* work);
	bool handleLoginResult(FutureWork<int64_t>* work);

public:
	inline Utils::Game::Session* session() { return &_session; }
	void onRead(NString packet);
	void sendError(std::string&& error);

private:
	Utils::Game::Session _session;
};
