#pragma once

#include <functional>
#include <string.h>

#include <Tools/socket.h>
#include <Tools/nstring.h>
#include <Tools/server_socket.h>
#include <Tools/accepted_socket.h>
#include <threadpool.h>

#ifdef _WIN32
	#define pollfd WSAPOLLFD
	#define poll WSAPoll
#endif

template <typename ClientType>
class Reactor
{
public:
	Reactor(ServerSocket* socket, int maxConnections, int timeout) :
		_socket(socket),
		_maxConnections(maxConnections),
		_timeout(timeout)
	{
		_polls = new pollfd[maxConnections + 1];
		memset(_polls, 0, (maxConnections + 1) * sizeof(pollfd));

		_polls[0].fd = _socket->sock();
		_polls[0].events |= POLLIN;

		_clients = new ClientType*[maxConnections + 1];
		memset(_clients, 0, (maxConnections + 1) * sizeof(ClientType*));
	}

	~Reactor()
	{
		delete[] _polls;
		delete[] _clients;
	}

	bool start(int port)
	{
		if (_socket->serve(port))
		{
			ThreadPool::get()->pool()->postWork<void>([this] {
				loopSockets(this);
			});

			return true;
		}

		return false;
	}

	static void loopSockets(Reactor<ClientType>* reactor)
	{
		while (reactor->_running)
		{
			poll(reactor->_polls, reactor->_maxConnections, reactor->_timeout);

			if (reactor->_polls[0].revents & POLLIN)
			{
				ClientType* client = new ClientType();
				client = (ClientType*)reactor->_socket->accept(client);

				if (client)
				{
					int currentFree = 1;
					while (currentFree < reactor->_maxConnections && reactor->_clients[currentFree] != nullptr)
					{
						++currentFree;
					}

					if (currentFree >= reactor->_maxConnections)
					{
						std::cout << "Dropping user, not enough slots" << std::endl;
						client->close();
					}
					else
					{
						std::cout << "Connection in " << currentFree << std::endl;

						reactor->_clients[currentFree] = client;
						reactor->_polls[currentFree].fd = client->sock();
						reactor->_polls[currentFree].events = POLLIN;
					}
				}
			}

			for (int i = 1; i < reactor->_maxConnections; ++i)
			{
				bool cleanup = reactor->_polls[i].fd > 0;

				if (reactor->_clients[i] && (
					reactor->_clients[i]->status() == SocketStatus::CONNECTED ||
					reactor->_clients[i]->status() == SocketStatus::CLOSING))
				{
					cleanup = false;

					if (reactor->_clients[i]->status() != SocketStatus::CLOSING)
					{
						if (reactor->_polls[i].revents & POLLIN)
						{
							NString packet = reactor->_clients[i]->recv();
							if (!packet.empty())
							{
								reactor->_clients[i]->onRead(packet);
							}
						}
					}

					if (reactor->_polls[i].revents & POLLOUT)
					{
						reactor->_clients[i]->onWrite();
						reactor->_polls[i].events &= ~POLLOUT;
					}

					if (!reactor->_clients[i]->_sendQueue.empty())
					{
						reactor->_polls[i].events |= POLLOUT;
					}
					else if (reactor->_clients[i]->status() == SocketStatus::CLOSING)
					{
						reactor->_clients[i]->close();
					}
				}

				if (cleanup)
				{
					std::cout << "Disconnected " << i << std::endl;
					reactor->_polls[i].fd = 0;
					reactor->_polls[i].events = 0;

					delete reactor->_clients[i];
					reactor->_clients[i] = nullptr;
				}
			}
		}
	}

private:
	ServerSocket* _socket;
	bool _running;
	int _maxConnections;
	int _timeout;

	pollfd* _polls;
	ClientType** _clients;
};
