// tt6.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "windows.h"
#include "string"
using namespace std;

LONG WINAPI myUnhandledExceptionFilter(_EXCEPTION_POINTERS* pExceptionInfo)
{
	MessageBox(0,_T("test"),_T("test"),IDOK);
	return EXCEPTION_EXECUTE_HANDLER;
}

void my_unexpected_function()
{
	MessageBox(0,_T("test"),_T("test"),IDOK);
}

void DisableSetUnhandledExceptionFilter()

{
#ifndef _M_IX86

#error "The following code only works for x86!"

#endif

	void *addr = (void*)GetProcAddress(LoadLibrary(_T("kernel32.dll")),

		"SetUnhandledExceptionFilter");

	if (addr)

	{

		unsigned char code[16];

		int size = 0;

		code[size++] = 0x33;

		code[size++] = 0xC0;

		code[size++] = 0xC2;

		code[size++] = 0x04;

		code[size++] = 0x00;



		DWORD dwOldFlag, dwTempFlag;

		VirtualProtect(addr, size, PAGE_READWRITE, &dwOldFlag);

		WriteProcessMemory(GetCurrentProcess(), addr, code, size, NULL);

		VirtualProtect(addr, size, dwOldFlag, &dwTempFlag);

	}

}

void my_invalid_parameter_handler(const wchar_t *, const wchar_t *, const wchar_t *, unsigned int, uintptr_t)
{
	MessageBox(0,_T("test"),_T("test"),IDOK);
}



// int _tmain(int argc, _TCHAR* argv[])
// {
// // 	
// // 
// // 	
// // 	
// // 	set_unexpected(&my_unexpected_function);
// // 
// // 	//_set_abort_behavior()
// // 
//  	_set_invalid_parameter_handler(&my_invalid_parameter_handler);
// // 
// // 	_set_abort_behavior(0,_WRITE_ABORT_MSG | _CALL_REPORTFAULT);
// // 
// // 	SetErrorMode(SEM_FAILCRITICALERRORS);
// 
// 	//_set_abort_behavior()
// 
// 	LPTOP_LEVEL_EXCEPTION_FILTER p2 = SetUnhandledExceptionFilter(&myUnhandledExceptionFilter);
// 
// 	DisableSetUnhandledExceptionFilter();
// 
// 	SetVectorExceptionHandler()
// 
// 	string * p = new string("test");
// 	delete p;
// 	//char buffer[22] = "test";
// 	//char a = buffer[26];
// 	int n1,n2;
// 	n1 = 12;
// 	n2 = 0;
// 	//int n3 = n1 / n2;
// 
// 	char *p1 = 0;
// 	*p1 = 5;
// 
// 	int n4 = atoi(0);
// 
// 	return 0;
// }
// 

int SehFilter(DWORD dwExceptionCode)
{
	switch (dwExceptionCode)
	{
	case EXCEPTION_ACCESS_VIOLATION:
		return EXCEPTION_EXECUTE_HANDLER;
	}
	return EXCEPTION_CONTINUE_SEARCH;
}

int test()
{
	*(int*)0 = 0;
	return 0;
}

int main(int argc, char* argv[])
{
	__try
	{
		test();
		printf("test()");
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		printf("EXCEPTION_ACCESS_VIOLATION");
	}

	getchar();
}