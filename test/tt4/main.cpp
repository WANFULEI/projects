#include "tt4.h"
#include <QtGui/QApplication>

class test2
{
public:
// 	virtual ~test2(){
// 		int n = 0;
// 	}
};

class test : public QObject , public test2
{
public:
	virtual ~test(){
		int n = 0;
	}
};

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	
// 	test2 * p = new test;
// 	delete p;

	tt4 w;
	w.show();

	return a.exec();
}
