#ifndef FUNCTIONS_H
#define FUNCTION_H
#include"pch.h"
#include <iostream>
#include<TlHelp32.h>
#include<Psapi.h>
/* // MessageBox wrapper to display address
void MsgBoxAddy(DWORD addy)
{
	char szBuffer[1024];
	sprintf_s(szBuffer, "Addy:%02x", addy);
	MessageBoxA(NULL, szBuffer, "Title", MB_OK);

}   */

MODULEINFO GetModuleInfo(const char* szModule)
{
	MODULEINFO modInfo { 0 };
	HMODULE hModule = ::GetModuleHandleA(szModule);
	if (hModule == 0)
		return modInfo;
	GetModuleInformation(GetCurrentProcess(), hModule, &modInfo, sizeof(MODULEINFO));
	return modInfo;
}

void WriteToMemory(uintptr_t addressToWrite, char* valueToWrite, int byteNum)
{
	unsigned long oldProtection {};
	unsigned long oldProtection1 {};
	VirtualProtect((LPVOID) addressToWrite, byteNum, PAGE_EXECUTE_READWRITE, &oldProtection);
	RtlMoveMemory((LPVOID) addressToWrite, valueToWrite, byteNum);
	VirtualProtect((LPVOID) addressToWrite, byteNum, oldProtection, &oldProtection1);
}

DWORD FindPattern(const char* module, const char* pattern, const char* mask)
{
	MODULEINFO mInfo = ::GetModuleInfo(module);// get moduleinfo structure
	DWORD base = (DWORD) mInfo.lpBaseOfDll;// base address of module
	DWORD size = (DWORD) mInfo.SizeOfImage; // size of module 
	DWORD patternLength = (DWORD) strlen(mask);
	for (DWORD i = 0; i < size - patternLength; ++i)
	{

		bool found = true;
		for (DWORD j = 0; j < patternLength; ++j)
		{
			found &= (mask[j] == '?') || (pattern[j] == *(char*) (base + i + j));
		}
		//found = true, our entire pattern was found
		// return the memory address so we acn write it
		if (found)
		{
			return base + i;
		}
	}
	return 0xBADF00D;
}

#endif

