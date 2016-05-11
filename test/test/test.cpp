// test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include<iostream>
#include<Windows.h>
#include<Uxtheme.h>
#include<dwmapi.h>
using namespace std;
#pragma comment(lib,"dwmapi.lib")
int main()
{
	int aa[3][3] = { 1 , 2 , 3 ,
					 4 , 5 , 6 ,
					 7 , 8 , 9 };

	cout << aa[0][1] << endl;
	cout << aa[1][0] << endl;

	for( int i = 0 ; i < 9 ; ++i )
	{
		cout << ( ( int * )( aa ) )[ i ] << endl;
	}

	char * ss = "weiqing "
				"is "
				"a "
				"fool ";

	cout << ss << endl;

MARGINS mar={-1,-1,-1,-1};
///< 窗口句柄
HWND hWnd=GetConsoleWindow();

///< 指定窗口实现毛玻璃
DwmExtendFrameIntoClientArea(hWnd,&mar);

///< 
int i;
 cin >> i;
return 0;
}
