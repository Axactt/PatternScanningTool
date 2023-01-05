// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include"TrampClass.h"
#include<iostream>
#include "PatternScanner.h"

char processName[] = "Doom3BFG.exe";

void WINAPI  MyThreadFunction1()
{
	char bytesToPatch1[] = "\x90\x90";
	char bytePattern1[] = "\x29\x0E\xB9\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x85\xC0\x74\x13\xB9\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x8B\x90\x00\x00\x00\x00\x89\x56\x04";
	char byteMask1[] = "xxx????x????xxxxx????x????xx????xxx";
	int codePosition1 = 0;
	DWORD logAddress = FindPattern("Doom3BFG.exe", bytePattern1, byteMask1) ;
	logAddress += codePosition1; // To shift to actual address of reading and patching
	std::cout <<"InfiniteAmmo Address2: " << std::hex << logAddress << '\n';
	WriteToMemory(logAddress, bytesToPatch1, 2);

}

/*============================================================================================================================================================*/
// To log address of class instance idGameLocal
DWORD addressLogger2() 
{
	char bytePattern2[] = "\xB9\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x85\xC0\x0F\x84\x00\x00\x00\x00\xD9\x06\xA1\x00\x00\x00\x00\xD8\x07";
	char byteMask2[] = "x????x????xxxx????xxx????xx";
	int codePosition2 = 1;
	DWORD logAddress2 = FindPattern(processName, bytePattern2, byteMask2);
	logAddress2 += codePosition2; // To shift to actual address of reading and patching
	return logAddress2;
}

void WINAPI MyThreadFunction2()
{
	uintptr_t modBase = (uintptr_t)::GetModuleHandle(NULL);
	std::cout <<"[+] Module Base    " << std::hex<<modBase << '\n';
	std::cout << "   --> " << "idGameLocal\t" << std::hex<<*(uintptr_t*)addressLogger2() << '\n'; // read the offset(class pointer) at that particular adddress

}

/*============================================================================================================================================================*/
// To log address of class member function idGameLocal::GetLocalPlayer
DWORD addressLogger3() 
{
	char bytePattern3[] = "\x56\x8B\xF1\x8B\x06\x8B\x50\x7C\xFF\xD2\x85\xC0\x78\x50\x8B\x06\x8B\x50\x7C\x8B\xCE\xFF\xD2\x83\xBC\x86\x00\x00\x00\x00\x00";
	char byteMask3[] = "xxxxxxxxxxxxxxxxxxxxxxxxxx?????";
	int codePosition3 = 0;
	DWORD logAddress3 = FindPattern(processName, bytePattern3, byteMask3);
	logAddress3 += codePosition3; // To shift to actual address of reading and patching
	return logAddress3;
}

void WINAPI MyThreadFunction3()
{

	std::cout << "   --> " << "GetLocalPlayer\t" << std::hex << addressLogger3() << '\n';

}

/*============================================================================================================================================================*/

// here we make a dummy call to idGameLocal::GetLocalPlayer() __thisCall function 
// Create a "idPlayer* idGameLocal::GetLocalPlayer() const"  hooking function prototype in __fastcall way

// Function pointer(type-casted to original game function address) with its using alias ::

using PFuncPtrAlias = uintptr_t * (__thiscall*) (void*);
PFuncPtrAlias GetLocalPlayerFuncPtr = (PFuncPtrAlias) addressLogger3();

// hooking function
uintptr_t* __fastcall GetLocaLPlayerDummy(void* pECX, void* pUnUsedEDX) // a dummy function prototype to call __thiscall function with its function pointer
{
	return  GetLocalPlayerFuncPtr(pECX);
}


void WINAPI MyThreadFunction4()
{
	uintptr_t pECX = *(uintptr_t*) addressLogger2(); // We assign a variable pECX to address of this pointer of Class Instance idGameLocal

	std::cout << "   --> " << "localPlayer\t" << std::hex << GetLocaLPlayerDummy((void*)pECX,NULL) << '\n';

}

/*============================================================================================================================================================*/
// Here we will hook idWeapon::GetWeaponDef to get the custon rapid-fire
// We shall change the weapon-def to machine-gun for all guns. 
// this will emulate the rapidfire for all weapons












BOOL APIENTRY DllMain(HMODULE hModule,
					  DWORD  fdwReason,
					  LPVOID lpReserved
)
{
	switch (fdwReason)
	{
		case DLL_PROCESS_ATTACH:

		// MessageBoxA(NULL, " Attached Successfully", "", 0);
		//CreateThread(nullptr, 0, MyThreadFunction, nullptr, 0, nullptr);
		AllocConsole();
		FILE* f;
		freopen_s(&f, "CONOUT$", "w", stdout);
		MyThreadFunction1();
		MyThreadFunction2();
		MyThreadFunction3();
		MyThreadFunction4();
		 ::DisableThreadLibraryCalls(hModule);
		break;

		default:

		break;
	}
	return TRUE;
}




