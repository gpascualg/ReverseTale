#include <iostream>
#include <string>
#include <sstream>
#include <time.h>
#include <functional>

#include <boost/lockfree/queue.hpp>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/options/find.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

#include <Cryptography/login.h>
#include <Cryptography/game.h>
#include <Game/packet.h>
#include <Tools/server_socket.h>
#include <Tools/accepted_socket.h>
#include <Tools/utils.h>
#include <reactor.h>

#include <INIReader.h>

#include <ReverseTale.h>
#if HAVE_EXPERIMENTAL_FS
	#include <experimental/filesystem>
	
	namespace fs = std::experimental::filesystem;
#else
	#include <Tools/filesystem.h>
#endif


#ifndef _WIN32
	#define Sleep sleep
#endif

#include "asyncwork.h"
#include "client.h"


using namespace Net;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::finalize;


mongocxx::v_noabi::database db;

class Client;

boost::lockfree::queue<AbstractWork*> asyncWork(2048);


int main(int argc, char** argv)
{
	srand((uint32_t)time(NULL));

	for (int i = 0; i < (((float)rand() / RAND_MAX) + 1) * 100; ++i)
	{
		Utils::seedRandom(0x989680);
	}


	ServerSocket socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	//socket.serve(4005); // IP: 100007F

	mongocxx::instance inst{};
	mongocxx::client conn{ mongocxx::uri{} };

	db = conn["login"];

	bsoncxx::builder::stream::document filter_builder2;
	filter_builder2 << "_id" << "blipi" << "password" << "123qwe";
	auto cursor = db["users"].find(filter_builder2.view());
	for (auto&& doc : cursor) {
		std::cout << bsoncxx::to_json(doc) << std::endl;
	}

	Reactor<Client> reactor(&socket, 128, 100);
	reactor.start(4005);

	while (true)
	{
		while (!asyncWork.empty())
		{
			AbstractWork* work;
			if (asyncWork.pop(work))
			{
				(*work)();
			}
		}

		// TODO: Have a constant heartbeat
		Sleep(100);
	}

	/*
	{
		std::cout << "Accepting client" << std::endl;
		AcceptedSocket* client = socket.accept();
		if (client)
		{
			std::cout << "Client accepted" << std::endl;

			client->setOption(SOL_SOCKET, SO_REUSEADDR, NULL, 0);
			client->setOption(IPPROTO_TCP, TCP_NODELAY, NULL, 0);
			client->setUserData(new Client());

			Packet* loginPacket = gFactory->make(PacketType::SERVER_LOGIN, client->recv());
			auto packets = loginPacket->decrypt();
			for (auto data : packets)
			{
				std::cout << ">> " << data << std::endl;
			}

			auto tokens = Utils::tokenize(packets[0]);
			std::string& user = tokens[2];
			std::string& pass = tokens[3];
			pass = Utils::Login::decryptPass(pass);

			Packet* gameServers = gFactory->make(PacketType::SERVER_LOGIN);
			
			std::cout << "Try log as ." << user << ". ." << pass << "." << std::endl;

			bsoncxx::builder::stream::document filter_builder;
			filter_builder << "_id" << user << "password" << pass;
			if (db["users"].count(filter_builder.view()) > 0)
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
				*gameServers << "fail usuario y/o contraseña incorrectos" << (uint8_t)0xA;
			}

			std::cout << "<< " << gameServers->data() << std::endl;

			gameServers->send(client);
			client->close();
		}
	}

	{
		ServerSocket<Client*> socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
		socket.serve(4006); // IP: 100007F

		AcceptedSocket<Client*>* client = socket.accept();
		if (client)
		{
			std::cout << "Client accepted" << std::endl;

			client->setOption(SOL_SOCKET, SO_REUSEADDR, NULL, 0);
			client->setOption(IPPROTO_TCP, TCP_NODELAY, NULL, 0);
			client->setUserData(new Client());

			Packet* sessionPacket = gFactory->make(PacketType::SERVER_GAME, client->data()->session(), client->recv());
			
			auto packets = sessionPacket->decrypt();
			if (packets.size() != 1)
			{
				client->close();
				return 1;
			}

			auto tokens = Utils::tokenize(packets[0]);
			if (tokens.size() != 2)
			{
				client->close();
				return 1;
			}

			client->data()->session()->setAlive(Utils::decimal_str2hex(tokens[0]) + 1);
			client->data()->session()->setID(tokens[1]);

			gFactory->recycle(sessionPacket);

			Packet* userPassPacket = gFactory->make(PacketType::SERVER_GAME, client->data()->session(), client->recv());
			packets = userPassPacket->decrypt();

			if (packets.size() != 2)
			{
				client->close();
				return 1;
			}

			tokens = Utils::tokenize(packets[0]);
			if (tokens.size() < 2)
			{
				client->close();
				return 1;
			}
			if (!client->data()->session()->check_alive(tokens[0]))
			{
				client->close();
				return 1;
			}

			std::cout << packets[0] << std::endl;

			tokens = Utils::tokenize(packets[1]);
			if (tokens.size() < 2)
			{
				client->close();
				return 1;
			}
			if (!client->data()->session()->check_alive(tokens[0]))
			{
				client->close();
				return 1;
			}

			std::cout << packets[1] << std::endl;

			Packet* clist_start = gFactory->make(PacketType::SERVER_GAME, client->data()->session());
			*clist_start << "clist_start 0";
			clist_start->send(client);

			Packet* clist_end = gFactory->make(PacketType::SERVER_GAME, client->data()->session());
			*clist_end << "clist_end";
			clist_end->send(client);
		}

		getchar();
	}
	*/
	
	getchar();
	return 0;
}
