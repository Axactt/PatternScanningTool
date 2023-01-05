
#ifndef TRAMPCLASS_H
#define TRAMPCLASS_H
#include"pch.h"

unsigned int g_minJmpOpCodeLen { 5 }; // minimum length of jump opcode instructions

class TrampHook
{

private:
	uintptr_t m_origFuncAddress {};
	uintptr_t m_finalHookAddress {};
	uintptr_t m_stolenByteLength {};

public:

	TrampHook() = default; // default constructor

	//Constructor taking original address, finalHookaddress and stolenByteLength
	TrampHook(uintptr_t origFuncAddress, uintptr_t finalHookAddress, uintptr_t stolenByteLength) :
		m_origFuncAddress { origFuncAddress },
		m_finalHookAddress { finalHookAddress },
		m_stolenByteLength { stolenByteLength }
	{

	}
	bool Detour32()
	{
		if (m_stolenByteLength < g_minJmpOpCodeLen)
			return false;
		DWORD curProtection {};
		DWORD curProtection1 {};
		VirtualProtect((LPVOID) m_origFuncAddress, m_stolenByteLength, PAGE_EXECUTE_READWRITE, &curProtection);

		RtlFillMemory((BYTE*) m_origFuncAddress, m_stolenByteLength, 0x90); // this avoids having  stray bytes by filling with Nops

		uintptr_t relativeAddress = (uintptr_t) ((uintptr_t) m_finalHookAddress - (uintptr_t) m_origFuncAddress) - g_minJmpOpCodeLen;

		*(BYTE*) m_origFuncAddress = 0XE9; // jumpopcode

		*(uintptr_t*) ((uintptr_t) m_origFuncAddress + 1) = relativeAddress; // putting relative address after junp opcode

		VirtualProtect((LPVOID) m_origFuncAddress, m_stolenByteLength, curProtection, &curProtection1);

		return true;

	}

	BYTE* TrampHook32()
	{
		//if (stolenByteLength < g_minJmpOpCodeLen)
			//return false;

		void* gateWay = VirtualAlloc(NULL, m_stolenByteLength + g_minJmpOpCodeLen, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		if (gateWay != NULL)
			RtlMoveMemory(gateWay, (BYTE*) m_origFuncAddress, m_stolenByteLength);

		uintptr_t gateWayRelativeAddr =
			((uintptr_t) (m_origFuncAddress) +m_stolenByteLength)
			- ((uintptr_t) (gateWay) +m_stolenByteLength) - g_minJmpOpCodeLen;
		// jump opcode after copied bytes
		*(uintptr_t*) ((uintptr_t) (gateWay) +m_stolenByteLength) = 0xE9;

		// writing relative address to jump in gateWay memory alloaction after jump opcode
		*(uintptr_t*) ((uintptr_t) (gateWay) +m_stolenByteLength + 1) = gateWayRelativeAddr;

		Detour32();

		return (BYTE*) gateWay;
	}

};

#endif

