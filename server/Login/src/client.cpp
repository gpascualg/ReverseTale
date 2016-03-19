#include "client.h"
#include "asyncwork.h"

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


extern mongocxx::v_noabi::database db;
extern boost::lockfree::queue<AbstractWork*> asyncWork;

bool Client::handleReadLogin(ClientWork* work)
{
	auto tokens = Utils::tokenize(work->packet());
	std::string& user = tokens[2];
	std::string& pass = tokens[3];
	pass = Utils::Login::decryptPass(pass);

	std::cout << "Try log as ." << user << ". ." << pass << "." << std::endl;

	std::future<int64_t> future = ThreadPool::get()->pool()->postWork<int64_t>([user, pass]() {
		bsoncxx::builder::stream::document filter_builder;
		filter_builder << "_id" << user << "password" << pass;
		return db["users"].count(filter_builder.view());
	});

	asyncWork.push(new FutureWork<int64_t>(this, MAKE_WORK(&Client::handleLoginResult), std::move(future)));
	return true;
}

bool Client::handleLoginResult(FutureWork<int64_t>* work)
{
	Packet* gameServers = gFactory->make(PacketType::SERVER_LOGIN);

	if (work->get() > 0)
	{
		*gameServers << "NsTeST " << "12345" << " ";
		*gameServers << "127.0.0.1:4006:0:1.1.Prueba ";
		*gameServers << "127.0.0.1:4007:4:1.2.Prueba ";
		*gameServers << "127.0.0.1:4008:8:1.3.Prueba ";
		*gameServers << "127.0.0.1:4009:18:1.4.Prueba "; // < 4 = RECOMENDADO
		*gameServers << "127.0.0.1:4010:19:1.5.Prueba "; // 4 = NORMAL
		*gameServers << "-1:-1:-1:-1:10000.10000.4" << (uint8_t)0xA;

		// 0-3 (Recomendado)
		// 4-12 (Normal)
		// 12-18 (Derramar)
		// 19+ (Completo)
	}
	else
	{
		*gameServers << "fail Usuario y/o contraseña incorrectos\nPrueba de nuevo!" << (uint8_t)0xA;
	}

	std::cout << "<< " << gameServers->data() << std::endl;

	gameServers->send(this);
	close();
	return true;
}

void Client::onRead(std::string packet)
{
	Packet* loginPacket = gFactory->make(PacketType::SERVER_LOGIN, packet);
	auto packets = loginPacket->decrypt();
	for (auto data : packets)
	{
		std::cout << ">> " << data << std::endl;
		std::function<bool(Client*, AbstractWork*)> f = MAKE_WORK(&Client::handleReadLogin);

		asyncWork.push(new ClientWork(this, MAKE_WORK(&Client::handleReadLogin), data));
	}
}
