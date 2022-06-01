#pragma once

#define QWORD unsigned long long

#ifdef _M_IX86
	#define T DWORD
	#define PT PDWORD
#else
	#define T QWORD
	#define PT QWORD*
#endif