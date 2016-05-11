
#include <QtCore/QCoreApplication>

#include "../../base/baseset2/baseset2.h"
using namespace baseset;
#include <iostream>
using namespace std;

class test_share_obj : public share_obj
{
public:
	test_share_obj(){
		cout << "construct test_share_obj" << endl;
	}
	~test_share_obj(){
		cout << "deconstruct test_share_obj" << endl;
	}

	void test(){
		cout << "test" << endl;
	}
	
};

void test_share_ptr()
{
	test_share_obj a1;
	test_share_obj * b = new test_share_obj;
	delete b;
	share_ptr<test_share_obj> p1 = new test_share_obj;
	cout << "ref_count:" << p1->ref_count() << endl;
	share_ptr<test_share_obj> p2(p1);
	cout << "ref_count:" << p2->ref_count() << endl;
	share_ptr<test_share_obj> p3 = p2;
	cout << "ref_count:" << p3->ref_count() << endl;
	p3->test();
}

class test_instance : public instance<test_instance>{};

class test_list_vector_manager : public share_list_vector_manager<test_share_obj>
{
public:

};

class test_map_manager : public share_map_manager<int,test_share_obj>{};

void test_share_list_vector_manager()
{
	test_list_vector_manager m;
	share_ptr<test_share_obj> p1 = m.create_entry();
	cout << "ref_count:" << p1->ref_count() << endl;
	share_ptr<test_share_obj> p2 = p1;
	cout << "ref_count:" << p2->ref_count() << endl;
	share_ptr<test_share_obj> p3 = m.detach_entry(p1);

	m.clear();
	cout << "ref_count:" << p2->ref_count() << endl;
}

void test_share_map_manager()
{
	test_map_manager m;
	m.create_entry(1);
	share_ptr<test_share_obj> p2 = m.detach_entry(1);

	//ref count == 0
	//test_share_obj * p2 = m.detach_entry(1);
}

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	test_instance::get_instance();

	test_share_ptr();

	test_share_list_vector_manager();

	test_share_map_manager();



	return a.exec();
}
