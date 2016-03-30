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

bool Client::handleReadLogin(ClientWork* work)
{
	if (work->packet().tokens().length() >= 4)
	{
		std::string user = work->packet().tokens().str(2);
		std::string pass = work->packet().tokens().str(3);

		if (!user.empty() && pass.size() >= 3)
		{
			pass = Utils::Login::decryptPass(pass);

			std::cout << "Try log as ." << user << ". ." << pass << "." << std::endl;

			std::future<int64_t> future = gDB("login")->query<int64_t>([user, pass](mongocxx::database db) {
				bsoncxx::builder::stream::document filter_builder;
				filter_builder << "_id" << user << "password" << pass;
				int64_t result = db["users"].count(filter_builder.view());

				if (result > 0)
				{
					result = Utils::seedRandom(0x7623);

					bsoncxx::builder::stream::document update_builder;
					update_builder << "$set" << open_document << "session_id"
						<< (uint16_t)result << close_document;

					db["users"].update_one(filter_builder.view(), update_builder.view());
				}
				else
				{
					result = -1;
				}

				return result;
			});

			asyncWork.push(new FutureWork<int64_t>(this, MAKE_WORK(&Client::handleLoginResult), std::move(future)));
			return true;
		}
	}

	return false;
}

bool Client::handleLoginResult(FutureWork<int64_t>* work)
{
	int64_t sessionID = work->get();

	if (sessionID >= 0)
	{
		Packet* gameServers = gFactory->make(PacketType::SERVER_LOGIN);
		*gameServers << "NsTeST " << Utils::hex2decimal_str((uint16_t)sessionID) << " ";
		*gameServers << "127.0.0.1:4006:0:1.1.Prueba ";
		*gameServers << "127.0.0.1:4007:4:1.2.Prueba ";
		*gameServers << "127.0.0.1:4008:8:1.3.Prueba ";
		*gameServers << "127.0.0.1:4009:18:1.4.Prueba ";
		*gameServers << "127.0.0.1:4010:19:1.5.Prueba ";
		*gameServers << "-1:-1:-1:-1:10000.10000.4";

		std::cout << "<< " << gameServers->data().get() << std::endl;

		gameServers->send(this);
		
		// 0-3 (Recomendado)
		// 4-12 (Normal)
		// 12-18 (Derramar)
		// 19+ (Completo)
	}
	else
	{
		sendError("Usuario y/o contraseña incorrectos\rPrueba de nuevo!");
	}

	close();
	return true;
}

void Client::sendError(std::string&& error)
{
	Packet* errorPacket = gFactory->make(PacketType::SERVER_LOGIN, NString("fail ") << error);
	errorPacket->send(this);
}

void Client::onRead(NString packet)
{
	Packet* loginPacket = gFactory->make(PacketType::SERVER_LOGIN, packet);
	auto packets = loginPacket->decrypt();
	for (auto data : packets)
	{
		std::cout << ">> " << data.get() << std::endl;
		asyncWork.push(new ClientWork(this, MAKE_WORK(&Client::handleReadLogin), data));
	}
}
