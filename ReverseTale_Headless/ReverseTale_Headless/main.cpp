#include <iostream>
#include <string>
#include <sstream>
#include <time.h>
#include <Windows.h>

#include "md5.h"


std::string hex2str(BYTE w)
{
	char hexString[3];
	sprintf(hexString, "%.2X", w);
	hexString[2] = '\0';
	return std::string(hexString);
}


std::string hex2str(WORD w)
{
	char hexString[5];
	sprintf(hexString, "%.4X", w);
	hexString[4] = '\0';
	return std::string(hexString);
}

std::string hex2str(DWORD w)
{
	char hexString[9];
	sprintf(hexString, "%.8X", w);
	hexString[8] = '\0';
	return std::string(hexString);
}


DWORD _anterior = 0;
DWORD _operando2 = 0x8088405;
DWORD seedRandom(DWORD p0)
{
	DWORD resultado = (DWORD)((int)_anterior * (int)_operando2);
	++resultado;
	_anterior = resultado;
	uint64_t mult = (uint64_t)p0 * (uint64_t)resultado;
	return (DWORD)(mult >>  32);
}


std::string generateRandom1(DWORD p0)
{
	const int len = 7;

	BYTE* buffer = new BYTE[len];
	memset(buffer, 0, len);
	BYTE* ESI = buffer + len;

	DWORD ECX = 0x0A;
	DWORD EDX = 0;

	while (p0 != 0)
	{
		DWORD EAX = p0 / ECX;
		EDX = p0 % ECX;
		p0 = EAX;
		--ESI;

		BYTE DL = (EDX & 0xFF);
		DL += 0x30;

		if (DL >= 0x3A)
		{
			DL += 0x07;
		}

		*ESI = DL;
	}

	std::string randomNumber;
	for (int i = 0; i < len; ++i)
	{
		randomNumber += buffer[i];
	}
	
	delete[] buffer;
	return randomNumber;
}

BYTE password_table[] = { 
	0x2E, 0x2A, 0x17, 0x4F, 0x20, 0x24, 0x47, 0x11, 
	0x5B, 0x37, 0x53, 0x43, 0x15, 0x34, 0x45, 0x25, 
	0x4B, 0x1D, 0x2F, 0x58, 0x2B, 0x32, 0x63 
};


std::string encryptPass(std::string& password)
{
	int i = seedRandom(0x17);
	const char* pass = password.c_str();
	std::string hash;

	for (int n = 0; n < password.length(); ++n)
	{
		BYTE actual = pass[n];
		BYTE tabla = password_table[i];

		// tabla | actual

		/*
		WORD hash_i = ((tabla & 0xF0) | ((actual & 0xF0) >> 4)) << 8;
		hash_i |= ((tabla & 0x0F) << 4) | (actual & 0x0F);
		hash_i = ((hash_i & 0xFF) << 8) | ((hash_i >> 8) & 0xFF);
		*/
		WORD hash_i = (((tabla & 0x0F) << 4) | (actual & 0x0F)) << 8;
		hash_i |= (tabla & 0xF0) | ((actual & 0xF0) >> 4);

		hash += hex2str(hash_i);

		i = (++i) % 23;
	}

	return hash;
}

std::string login(std::string username, std::string password)
{
	MD5 md5;

	std::string basePath("P:\\Program Files (x86)\\GameforgeLive\\Games\\ESP_spa\\NosTale");
	std::string md5_nostaleX = md5.digestFile((basePath + "\\NostaleX.dat").c_str());	// 3D78AC41C49B735EFEE2008E2E0F1ED6
	std::string md5_nostale = md5.digestFile((basePath + "\\Nostale.dat").c_str());		// 9D07DAD6A3D2EFCF97630E8DF6FC4724
	
	std::string packet("NoS0575 ");
	packet += generateRandom1(seedRandom(0x989680) + 0x86111);
	packet += std::string(" ") + username;
	packet += std::string(" ") + encryptPass(password);
	packet += std::string(" ") + hex2str(seedRandom(0x989680));
	packet += 0x0B;
	packet += "0.9.3.3054 0 ";
	packet += md5.digestString(md5_nostaleX + md5_nostale + username);
	packet += 0x0A;

	return packet;
}

std::string encrytPacket(std::string& packet)
{
	for (int i = 0; i < packet.length(); ++i)
	{
		packet[i] = (packet[i] ^ 0xC3) + 0x0F;
	}

	return packet;
}

int main()
{
	srand(time(NULL));

	for (int i = 0; i < (((float)rand() / RAND_MAX) + 1) * 100; ++i)
	{
		seedRandom(0x989680);
	}

	std::string packet = login("123qwe", "qwe");
	int packetLength = packet.length();
	std::cout << packet << std::endl;

	encrytPacket(packet);

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

	WSACleanup();
	return 0;

	getchar();
}
