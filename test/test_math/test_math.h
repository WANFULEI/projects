#ifndef TEST_MATH_H
#define TEST_MATH_H

#include <QtGui/QMainWindow>
#include "ui_test_math.h"

class test_math : public QMainWindow
{
	Q_OBJECT

public:
	test_math(QWidget *parent = 0, Qt::WFlags flags = 0);
	~test_math();

private:
	Ui::test_mathClass ui;
};

#endif // TEST_MATH_H
