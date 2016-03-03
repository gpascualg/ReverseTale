#include <iostream>
#include <string>
#include <sstream>
#include <time.h>

#include <Login/login.h>
#include <Cryptography/login.h>
#include <Cryptography/game.h>
#include <Game/game.h>
#include <Tools/socket.h>
#include <INIReader.h>

#include <ReverseTale.h>
#if HAVE_EXPERIMENTAL_FS
	#include <experimental/filesystem>
	
	namespace fs = std::experimental::filesystem;
#else
	#include <Tools/filesystem.h>
#endif

int main(int argc, char** argv)
{
	srand((uint32_t)time(NULL));

	for (int i = 0; i < (((float)rand() / RAND_MAX) + 1) * 100; ++i)
	{
		Login::seedRandom(0x989680);
	}

	fs::path dir = argv[0];
	INIReader reader(dir.remove_filename().string() + "/config/default.conf");
	if (reader.ParseError() < 0)
	{
		std::cout << "Couldn't load config file. Press [ENTER] to exit" << std::endl;
		getchar();
		return 1;
	}

	/*
	Game::Session session;
	std::string t = session.encryptPacket("8955 5893", 9);
	for (std::size_t i = 0; i < t.length(); ++i) printf("%.2X ", (uint8_t)t[i]);

	session.setID("5893");
	t = session.encryptPacket("8955 blipi", 10);
	for (std::size_t i = 0; i < t.length(); ++i) printf("%.2X ", (uint8_t)t[i]);
	getchar();
	*/

	Login::setNostalePath("P:\\Program Files (x86)\\GameforgeLive\\Games\\ESP_spa\\NosTale");
	if (reader.HasKey("MD5", "nostaleX"))
	{
		Login::setMD5(reader.Get("MD5", "nostaleX", ""), reader.Get("MD5", "nostale", ""));
	}

	std::string packet = Login::login(reader.Get("Login", "User", "??"), reader.Get("Login", "Password", "??"));
	Crypto::Client::Login::Encrypter::get()->commit(packet);
	Crypto::Client::Login::Encrypter::get()->finish(packet);

	Socket socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	socket.setOption(SOL_SOCKET, SO_REUSEADDR, NULL, 0);
	socket.setOption(IPPROTO_TCP, TCP_NODELAY, NULL, 0);
	socket.connect("79.110.84.75", 4005);

	char* buf = new char[8192];
	int ret = socket.send(packet.c_str(), packet.length());
	int len = socket.recv(buf, 8192);

	std::string response(buf, len);
	Crypto::Client::Login::Decrypter::get()->parse(response);

	auto tokens = Game::tokenize(response);
	std::cout << "Login: " << response[0] << std::endl;

	// Connect to a random server!
	//socket.close();
	//socket.connect("xxxxx", 4005);

	Game::Session session;
	uint32_t alive = Login::seedRandom(0x989680);
	std::string content = Login::generateRandom1(alive) + std::string(" ") + tokens[1];

	Crypto::Client::Game::Encrypter::get()->commit(content);
	Crypto::Client::Game::Encrypter::get()->finish(content, &session);

	socket.send(content.c_str(), content.length());
	len = socket.recv(buf, 8192);

	packet = session.decryptRecv(buf, len);
	std::cout << "Server Answ: " << packet << std::endl;



	getchar();
	return 0;
}
