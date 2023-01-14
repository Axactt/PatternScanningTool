// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include"TrampHook.h"
#include<iostream>
#include "PatternScanner.h"

char processName[] = "Doom3BFG.exe";
uintptr_t jumpBackaddress { 0x729B35 };
uintptr_t origHookAddress { 0x729b2f };
uintptr_t m_stolenByteLength {6 };
uintptr_t registerValue {};

void __declspec(naked) registerLogger()
{
	_asm
	{ // misfunction changed asm code included here

		pushad
		mov registerValue, ecx
		mov[esi + 95Ch], eax
		popad
		jmp [jumpBackaddress]
	}

}
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
// To log addres of class member function idGameLocal::GetLocalPlayer
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

	std::cout << "   --> " << "localPlayer\t" << std::hex << GetLocalPlayerFuncPtr((void*)pECX) << '\n';// Function pointer of this call - called directly
	//Use of dummy __fastcall not required here as dummy doing nothing

}

/*============================================================================================================================================================*/

// here we will hook idGameLocal::FindEntityDef Class member function
// this function returns pointer to idDeclEntityDef class
DWORD addressLogger5()
{
	char bytePattern5[] = "\x55\x8B\xEC\x8B\x0D\x00\x00\x00\x00\x8B\x01\x8B\x50\x6C\x57\xFF\xD2\x8B\x7D\x08\x84\xC0\x74\x2B\xA1\x00\x00\x00\x00";
	char byteMask5[] = "xxxxx????xxxxxxxxxxxxxxxx????";
	int codePosition5 = 0;
	DWORD logAddress5 = FindPattern(processName, bytePattern5, byteMask5);
	logAddress5 += codePosition5; // To shift to actual address of reading and patching
	return logAddress5;
}

void WINAPI MyThreadFunction6()
{
	std::cout << "   --> " << "FindEntityDef\t" << std::hex << addressLogger5() << '\n';
}

// our function pointer to FindEntityDef function 

using FindEntityDefPtrAlias = void* (__thiscall*) (void*, const char*, bool);
FindEntityDefPtrAlias FindEntityDefPtr = (FindEntityDefPtrAlias) addressLogger5();

uintptr_t idGameLocalPThis = 0xd0b90;

// This FindEntityDefptr needs to be called inside GetWeaponDefDummy to get the weaponDefinition 

/*============================================================================================================================================================*/

// idDictPrint function address saved here
// to be changed to signature scan grabbing method later
uintptr_t idDictPrintaddress = 0x7543a0;


// idDictPrint function address saved here
// to be changed to signature scan grabbing method later
uintptr_t idDictsetAddress = 0x755e90;

// register logger function to set up

TrampHook magicClass1 { (uintptr_t) origHookAddress,(uintptr_t) &registerLogger, 6 };




/*============================================================================================================================================================*/
// Here we will hook idWeapon::GetWeaponDef to get the custon rapid-fire
// We shall change the weapon-def to machine-gun for all guns. 
// this will emulate the rapidfire for all weapons
DWORD addressLogger4()
{
	char bytePattern4[] = "\x55\x8B\xEC\x64\xA1\x00\x00\x00\x00\x6A\xFF\x68\x00\x00\x00\x00\x50\x64\x89\x25\x00\x00\x00\x00\x81\xEC\x00\x00\x00\x00\x56\x8B\xF1\xE8\x00\x00\x00\x00\x8B\x45\x08\x85\xC0";
	char byteMask4[] = "xxxxx????xxx????xxxx????xx????xxxx????xxxxx";
	int codePosition4 = 0;
	DWORD logAddress4 = FindPattern(processName, bytePattern4, byteMask4);
	logAddress4 += codePosition4; // To shift to actual address of reading and patching
	return logAddress4;
}

void WINAPI MyThreadFunction5()
{
	std::cout << "   --> " << "GetWeaponDef\t" << std::hex << addressLogger4() << '\n';
}
// logaddress4 will be address of our GetWeaponDef() function
// Now we need to create type-alias for our GetWeponDef func:: void idWeapon::GetWeaponDef( const char *objectname, int ammoinclip )

using GetWeaponDefPtrAlias = void (__thiscall*) (void*,const char*,int);
GetWeaponDefPtrAlias GetWeaponDefPtr = (GetWeaponDefPtrAlias) addressLogger4();

// our hooking function to print the present weapon name in __fastcall convention

void __fastcall GetWeaponDefDummy(void* pECX, void* pEDX, const char* objectname, int ammoclip)
{
			std::cout << "   --> " << "objectname\t" << objectname << '\n';
			void* weapondef = FindEntityDefPtr((void*) idGameLocalPThis, objectname, 1); // this returns current pointer instance of idDeclentitydef as weapondef
			std::cout <<"   --> " << "idDeclEntityDefPtr\t" << std::hex << weapondef<<'\n';
			uintptr_t idDictAddress = (uintptr_t) weapondef + 8;// this pointer to idDict class
			std::cout << "   --> " << "idDictPtr\t" << std::hex << idDictAddress << '\n';

			magicClass1.Detour32();
			std::cout << "Dynamic pTHIS for idDict Class::\t" << registerValue << '\n';
			// Setting function ptr for idDict::print function
			using idDictPrintPtrAlias = void (__thiscall*)(void*);
			idDictPrintPtrAlias  idDictPrintPtr = (idDictPrintPtrAlias) idDictPrintaddress; // function pointer for idDict::Print member function
			
			// Now call idDictPrint function to list all key-value pair
			idDictPrintPtr((void*) idDictAddress);

			// setting function pointer to call idDict::Set key
			using idDictSetPtrAlias = void(__thiscall*)(void*, const char*,const char*);
			idDictSetPtrAlias  idDictSetPtr = (idDictSetPtrAlias) idDictsetAddress; // function pointer for idDict::Print member function
			
			// Now call idDictSet function to list all key-value pair
			idDictSetPtr((void*) idDictAddress,"clipSize", "200");
			idDictSetPtr((void*) idDictAddress, "ammoRequired", "0");
			idDictSetPtr((void*) idDictAddress, "recoilAngles", "0 0 0");
			idDictSetPtr((void*) idDictAddress, "weaponAngleOffsetScale", "0");
			idDictSetPtr((void*) idDictAddress, "weapon_scriptobject", "weapon_machinegun");

			

						
	 GetWeaponDefPtr(pECX, objectname, ammoclip);
	
}

TrampHook magicClass { (uintptr_t) addressLogger4(),(uintptr_t) &GetWeaponDefDummy, 9 };

/*=============================================================================================================================================================*/
//code updated

/*=============================================================================================================================================================*/
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
		MyThreadFunction5();
		MyThreadFunction6();
		
/*===============================================================================================================================================================*/
		// original function pointer set/assigned to Trampoline function return value BYTE* gateway
		//This is done by c-style typecasting return value BYTE* gateway to GetWeaponDef function pointer using type-alias
		GetWeaponDefPtr = (GetWeaponDefPtrAlias) magicClass.TrampHook32();
/*===============================================================================================================================================================*/
		std::cout << '\n';
// Loggging dynamic register value

		


		 ::DisableThreadLibraryCalls(hModule);
		break;

		default:

		break;
	}
	return TRUE;
}




