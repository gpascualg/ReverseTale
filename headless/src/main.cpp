#include <iostream>
#include <string>
#include <sstream>
#include <time.h>
#include <Windows.h>

#include <Login/login.h>

int main()
{
	srand(time(NULL));

	for (int i = 0; i < (((float)rand() / RAND_MAX) + 1) * 100; ++i)
	{
		Login::seedRandom(0x989680);
	}

	std::string packet = Login::login("123qwe", "qwe");
	int packetLength = packet.length();
	std::cout << packet << std::endl;

	Login::encrytLogin(packet);

	for (int i = 0; i < packetLength; ++i)
	{
		printf("%.2X ", (BYTE)packet[i]);
	}

	//----------------------
	// Initialize Winsock
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		wprintf(L"WSAStartup function failed with error: %d\n", iResult);
		return 1;
	}
	//----------------------
	// Create a SOCKET for connecting to server
	SOCKET ConnectSocket;
	ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (ConnectSocket == INVALID_SOCKET) {
		wprintf(L"socket function failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	setsockopt(ConnectSocket, SOL_SOCKET, SO_REUSEADDR, NULL, 0);
	setsockopt(ConnectSocket, IPPROTO_TCP, TCP_NODELAY, NULL, 0);

	//----------------------
	// The sockaddr_in structure specifies the address family,
	// IP address, and port of the server to be connected to.
	sockaddr_in clientService;
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr("79.110.84.75");
	clientService.sin_port = htons(4005);

	//----------------------
	// Connect to server.
	iResult = connect(ConnectSocket, (SOCKADDR *)& clientService, sizeof(clientService));
	if (iResult == SOCKET_ERROR) {
		wprintf(L"connect function failed with error: %ld\n", WSAGetLastError());
		iResult = closesocket(ConnectSocket);
		if (iResult == SOCKET_ERROR)
			wprintf(L"closesocket function failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}


	int ret = send(ConnectSocket, packet.c_str(), packetLength, 0);
	wprintf(L"Connected to server.\n");

	char* buf = new char[8192];
	int len = recv(ConnectSocket, buf, 8192, 0);

	for (int i = 0; i < len; ++i)
	{
		printf("%.2X ", (BYTE)(buf[i] - 0xF));
	}
	printf("\n");
	for (int i = 0; i < len; ++i)
	{
		printf("%c", buf[i] - 0xF);
	}
	
	iResult = closesocket(ConnectSocket);
	if (iResult == SOCKET_ERROR) {
		wprintf(L"closesocket function failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	getchar();
	WSACleanup();
	return 0;
}
