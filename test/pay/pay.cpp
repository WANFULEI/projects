// pay.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <vector>
#include <iostream>
using namespace std;

#include <time.h>

float pay_one(float money,int persons)
{
	money *= 100;
	float pay = money / 2;
	float last = money - pay;
	while(last < persons - 1)
	{
		pay /= 2;
		last = money - pay;
	}
	pay = (rand() % int(pay)) + 1;
	return pay / 100;
}

vector<float> alloc(float money,int persons)
{
	vector<float> allocs;
	while(persons)
	{
		if(persons == 1)
		{
			allocs.push_back(money);
			break;
		}
		else if(persons == 2)
		{
			float pay = (rand() % int(money * 100 - 1)) + 1;
			allocs.push_back(pay / 100);
			allocs.push_back(money - pay / 100);
			break;
		}
		else if(persons >= 3)
		{
			float pay = pay_one(money,persons);
			allocs.push_back(pay);
			money -= pay;
			--persons;
		}
	}

	return allocs;
}

int _tmain(int argc, _TCHAR* argv[])
{
	srand(time(0));

	while(1)
	{
		float money = 0;
		int persons = 0;
		cout << "请输入多少钱？" << endl;
		cin >> money;

		cout << "请输入多少人？" << endl;
		cin >> persons;

		vector<float> pays = alloc(money,persons);
		for(int i = 0; i < pays.size(); ++i)
		{
			cout << pays[i] << endl;
		}

		cout << "输入y继续n退出" << endl;
		char c;
		cin >> c;
		if(c == 'n')
			break;
	}

	return 0;
}

