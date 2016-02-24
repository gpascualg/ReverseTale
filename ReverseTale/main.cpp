// Streaming23_02_16.cpp : Defines the exported functions for the DLL application.
//

#include "xHacking.h"
#include "Utilities/Utilities.h"
#include "Loader/Loader.h"
#include "Detour/Detour.h"


using namespace xHacking;


Detour<int, int, const char*, int, int>* sendDetour = NULL;
Detour<int, int, char*, int, int>* recvDetour = NULL;
int WINAPI nuestro_send(SOCKET s, const char *buf, int len, int flags)
{
	int ret = (*sendDetour)(s, buf, len, flags);

	__asm PUSHAD;
	__asm PUSHFD;

	printf("---> SEND Len: %X %d\nCrypted:\n", (void*)buf, len);

	for (int i = 0; i < len; ++i)
	{
		printf("%.2X ", (BYTE)buf[i]);
	}
	printf("\n:Decrypted hex:\n");
	for (int i = 0; i < len; ++i)
	{
		printf("%.2X ", (BYTE)((buf[i] - 0xF) ^ 0xC3));
	}
	printf("\nDecrypted chars:\n");
	for (int i = 0; i < len; ++i)
	{
		printf("%c", ((buf[i] - 0xF) ^ 0xC3));
	}
	printf("\n\n");

	__asm POPFD;
	__asm POPAD;

	return ret;
}

int WINAPI nuestro_recv(SOCKET s, char *buf, int len, int flags)
{
	int ret = (*recvDetour)(s, buf, len, flags);

	__asm PUSHAD;
	__asm PUSHFD;

	printf("---> RECV Len: %X %d\nCrypted:\n", (void*)buf, ret);

	for (int i = 0; i < ret; ++i)
	{
		printf("%.2X ", (BYTE)buf[i]);
	}

	printf("\n:Decrypted hex:\n");
	for (int i = 0; i < ret; ++i)
	{
		printf("%.2X ", (BYTE)(buf[i] - 0xF));
	}
	printf("\nDecrypted chars:\n");
	for (int i = 0; i < ret; ++i)
	{
		printf("%c", (buf[i] - 0xF));
	}
	printf("\n\n");

	__asm POPFD;
	__asm POPAD;

	return ret;
}

void Hooks()
{
	sendDetour = new Detour<int, int, const char*, int, int>();
	sendDetour->Wait("WS2_32.dll", "send", (BYTE*)nuestro_send);

	recvDetour = new Detour<int, int, char*, int, int>();
	recvDetour->Wait("wsock32.dll", "recv", (BYTE*)nuestro_recv);
}

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, DWORD reserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		// Use the xHacking::CreateConsole function
		CreateConsole();

		// Call our function
		Hooks();
	}

	return true;
}
