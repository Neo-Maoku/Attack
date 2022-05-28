#include <windows.h>
#include <stdio.h>
#include "Global.h"
#include "CmdlineParser.hpp"
#include "StackArgTool.hpp"

using namespace std;

void(*p_func)();

int stackCount = 0;
int stackActualIndex = -1;
T beginAddress;
StackArg* stack;

void test(T a, T b, T c, T d)
{
	stackCount++;
	
	if (stackCount == stack->stackTotal) {
#ifdef _M_IX86
		beginAddress = (T)((PT)&a) - stack->typeLength * 2;
#else
		beginAddress = (T)((PT)&a) - stack->typeLength * 1;
#endif
	}
	else if (stackCount < stack->fillCount) {
		test(0, 0, 0, 0);
	}
	else if(stackCount < stack->stackTotal){
		stackActualIndex++;
#ifdef _M_IX86
		test(stack->m_values[3][stackActualIndex], stack->m_values[2][stackActualIndex], stack->m_values[1][stackActualIndex], stack->m_values[0][stackActualIndex]);
#else
		test(stack->m_values[14][stackActualIndex], stack->m_values[13][stackActualIndex], stack->m_values[12][stackActualIndex], stack->m_values[11][stackActualIndex]);
#endif
	}
}

int main()
{
	LPSTR lpcmdline = GetCommandLineA();
	std::string strCmdLine = (lpcmdline == nullptr ? "" : lpcmdline);
	CmdlineParser cmdLine(strCmdLine);

	if (!cmdLine.hasParam("f"))
	{
		return TRUE;
	}

	stack = new StackArg(cmdLine["f"].toString());

	if (stack->m_status) {
		test(0, 0, 0, 0);

		for (int i = stack->stackTotal - 1, index = 0; i >= stack->fillCount; i--, index++) {
#ifdef _M_IX86
			*(PT)(beginAddress + index * stack->argSum * stack->typeLength) = stack->m_values[5][i - stack->fillCount];
			*(PT)(beginAddress + index * stack->argSum * stack->typeLength + stack->typeLength) = stack->m_values[4][i - stack->fillCount];
#else
			*(PT)(beginAddress + index * stack->argSum * stack->typeLength) = stack->m_values[stack->argSum - 1][i - stack->fillCount];
			for (size_t j = 0; j <= 10; j++)
			{
				*(PT)(beginAddress + index * stack->argSum * stack->typeLength + stack->typeLength * (5+j)) = stack->m_values[stack->argSum - 1 - 5-j][i - stack->fillCount];
			}
#endif
		}
#ifndef _M_IX86
		DWORD dwFalg;
		int status = VirtualProtect((LPVOID)beginAddress, 0x2000, 0x20, &dwFalg);
#endif

		p_func = (void(*)())(beginAddress);

		p_func();
	}
}
