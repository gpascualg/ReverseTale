#pragma once

#include <iostream>
#include <functional>
#include <future>


#define MAKE_WORK(x) (bool(__thiscall Client::*)(AbstractWork*))(x)

class Client;

struct AbstractWork
{
public:
	bool operator()()
	{
		return _handler(_client, this);
	}

	inline Client* client() { return _client; }

protected:
	AbstractWork(Client* client, std::function<bool(Client*, AbstractWork*)> handler) :
		_client(client),
		_handler(handler)
	{}

protected:
	Client* _client;
	std::function<bool(Client*, AbstractWork*)> _handler;
};

template <typename T>
struct FutureWork : public AbstractWork
{
public:
	FutureWork(Client* client, std::function<bool(Client*, AbstractWork*)> handler, std::future<T>&& future) :
		AbstractWork(client, handler)
	{
		_future = std::move(future);
	}

	inline T get() { return _future.get(); }

private:
	std::future<T> _future;
};

struct ClientWork : public AbstractWork
{
	ClientWork(Client* client, std::function<bool(Client*, AbstractWork*)> handler, std::string packet) :
		AbstractWork(client, handler)
	{
		_packet = packet;
	}

	inline std::string packet() { return _packet; }

private:
	std::string _packet;
};
