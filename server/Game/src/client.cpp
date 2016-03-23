#include "client.h"
#include "asyncwork.h"
#include "database.h"

#include <threadpool.h>
#include <Game/packet.h>
#include <threadpool11/threadpool11.hpp>

#include <boost/lockfree/queue.hpp>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/options/find.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

using namespace Net;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::finalize;


extern boost::lockfree::queue<AbstractWork*> asyncWork;


Client::Client() :
	_currentWork(MAKE_WORK(&Client::handleConnect))
{}

bool Client::workRouter(AbstractWork* work)
{
	// FIXME: Implement other packets!
	if (_currentWork != nullptr)
	{
		return (this->*_currentWork)(work);
	}

	return true;
}

bool Client::handleConnect(ClientWork* work)
{
	auto tokens = Utils::tokenize(work->packet());
	if (tokens.size() == 2)
	{
		_session.setAlive(Utils::decimal_str2hex(tokens[0]) + 1);
		_session.setID(tokens[1]);

		_currentWork = MAKE_WORK(&Client::handleUserCredentials);

		return true;
	}

	return false;
}

bool Client::handleUserCredentials(ClientWork* work)
{
	auto tokens = Utils::tokenize(work->packet());
	if (tokens.size() == 2)
	{
		_username = tokens[1];
		_currentWork = MAKE_WORK(&Client::handlePasswordCredentials);
		return true;
	}

	return false;
}

bool Client::handlePasswordCredentials(ClientWork* work)
{
	auto tokens = Utils::tokenize(work->packet());
	if (tokens.size() == 2)
	{
		std::string& password = tokens[1];

		std::cout << "User: " << _username << " PASS: " << password << std::endl;

		auto future = gDB("login")->query<bool>([this, password](mongocxx::database db) {
			bsoncxx::builder::stream::document filter_builder;
			filter_builder << "_id" << _username << "password" << password << "session_id" << (uint16_t)_session.id();

			return db["users"].count(filter_builder.view()) == 1;
		});

		asyncWork.push(new FutureWork<bool>(this, MAKE_WORK(&Client::sendConnectionResult), std::move(future)));

		return true;
	}

	return false;
}

bool Client::sendConnectionResult(FutureWork<bool>* work)
{
	if (work->get())
	{
		/*<< clist 0 Blipi 0 0 1 4 0 0 2 -1.12.1.8.-1.-1.-1.-1 1  1 1 -1.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1 0 0*/

		Packet* clist_start = gFactory->make(PacketType::SERVER_GAME, &_session, std::string("clist_start 200"));
		Packet* clist_0 = gFactory->make(PacketType::SERVER_GAME, &_session, std::string("clist 0 blipi 3 0 0 9 0 0 80 1.12.1.8.0.0.0.123 1  1 1 -1.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1 0 0"));
		Packet* clist_end = gFactory->make(PacketType::SERVER_GAME, &_session, std::string("clist_end"));
		
		clist_start->send(this);
		clist_0->send(this);
		clist_end->send(this);

		_currentWork = nullptr;

		return true;
	}
	else
	{
		sendError("User not found");
		return false;
	}
}

void Client::sendError(std::string&& error)
{
	Packet* errorPacket = gFactory->make(PacketType::SERVER_GAME, &_session, std::string("fail ") + error);
	*errorPacket << (uint8_t)0xA;
	errorPacket->send(this);
}

void Client::onRead(std::string packet)
{
	Packet* loginPacket = gFactory->make(PacketType::SERVER_GAME, &_session, packet);
	auto packets = loginPacket->decrypt();
	for (auto data : packets)
	{
		std::cout << ">> " << data << std::endl;
		asyncWork.push(new ClientWork(this, MAKE_WORK(&Client::workRouter), data));
	}
}
