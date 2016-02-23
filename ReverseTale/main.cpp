// Streaming23_02_16.cpp : Defines the exported functions for the DLL application.
//

#include "xHacking.h"
#include "Utilities/Utilities.h"
#include "Loader/Loader.h"
#include "Detour/Detour.h"


using namespace xHacking;


Detour<int, int, const char*, int, int>* sendDetour = NULL;
int WINAPI nuestro_send(SOCKET s, const char *buf, int len, int flags)
{
	int ret = (*sendDetour)(s, buf, len, flags);

	__asm PUSHAD;
	__asm PUSHFD;

	printf("Len: %X %d\nCrypted:\n", (void*)buf, len);

	for (int i = 0; i < len; ++i)
	{
		printf("%2X ", buf[i]);
	}
	printf("\n:Decrypted hex:\n");
	for (int i = 0; i < len; ++i)
	{
		printf("%2X ", ((buf[i] - 0xF) ^ 0xC3));
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

void Hooks()
{
	sendDetour = new Detour<int, int, const char*, int, int>();
	sendDetour->Wait("WS2_32.dll", "send", (BYTE*)nuestro_send);
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
