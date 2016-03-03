#include <iostream>
#include <string>
#include <sstream>
#include <time.h>

#include <Login/login.h>
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

	Game::Session session;
	std::string t = session.encryptPacket("8955 5893", 9);
	for (std::size_t i = 0; i < t.length(); ++i) printf("%.2X ", (uint8_t)t[i]);

	session.setID("5893");
	t = session.encryptPacket("8955 blipi", 10);
	for (std::size_t i = 0; i < t.length(); ++i) printf("%.2X ", (uint8_t)t[i]);
	getchar();

	std::string packet = Login::login(reader.Get("Login", "User", "??"), reader.Get("Login", "Password", "??"));
	int packetLength = packet.length();

	Login::encrytLogin(packet);

	Socket socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	socket.setOption(SOL_SOCKET, SO_REUSEADDR, NULL, 0);
	socket.setOption(IPPROTO_TCP, TCP_NODELAY, NULL, 0);
	socket.connect("79.110.84.75", 4005);

	char* buf = new char[8192];
	int ret = socket.send(packet.c_str(), packetLength);
	int len = socket.recv(buf, 8192);

	Login::decrytAnswer(buf, len);

	printf("RECV: \n");
	for (int i = 0; i < len; ++i)
	{
		printf("%.2X ", (uint8_t)buf[i]);
	}

	printf("\n");
	for (int i = 0; i < len; ++i)
	{
		printf("%c", buf[i]);
	}

	getchar();
	return 0;
}
