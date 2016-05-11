#include <stdio.h>

#include "stdafx.h"
#include <stdlib.h>
//using namespace std;


#pragma pack( push, 4 )

__declspec( align(32) )struct D
{
	int i1;
	double d1;
	int i2;
	int i3;
};

__declspec( align(16) ) struct E
{
	int i1;
	D m_d;
	int i2;
};

int main()
{
	cout << "sizeof(int) = "<<sizeof(int) << endl;
	cout << "sizeof(char) = " << sizeof(char) << endl;
	cout << "sizeof(double) = " << sizeof(double) << endl;
	cout << sizeof(D) << endl;
	cout << sizeof(E) << endl;
	system("PAUSE");
	return 0;
}