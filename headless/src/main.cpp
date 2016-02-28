#include <iostream>
#include <string>
#include <sstream>
#include <time.h>
#include <Windows.h>

#include <experimental/filesystem>

#include <Login/login.h>
#include <Tools/socket.h>
#include <INIReader.h>

namespace fs = std::experimental::filesystem;


int main(int argc, char** argv)
{
	srand(time(NULL));

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

	std::string packet = Login::login(reader.Get("Login", "User", "??"), reader.Get("Login", "Password", "??"));
	int packetLength = packet.length();
	std::cout << packet << std::endl;

	Login::encrytLogin(packet);

	for (int i = 0; i < packetLength; ++i)
	{
		printf("%.2X ", (BYTE)packet[i]);
	}

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
		printf("%.2X ", (BYTE)buf[i]);
	}

	printf("\n");
	for (int i = 0; i < len; ++i)
	{
		printf("%c", buf[i]);
	}

	getchar();
	return 0;
}
