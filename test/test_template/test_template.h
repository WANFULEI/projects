#ifndef TEST_TEMPLATE_H
#define TEST_TEMPLATE_H

#include <QtGui/QMainWindow>
#include "ui_test_template.h"

class test_template : public QMainWindow
{
	Q_OBJECT

public:
	test_template(QWidget *parent = 0, Qt::WFlags flags = 0);
	~test_template();

private:
	Ui::test_templateClass ui;
};

#endif // TEST_TEMPLATE_H
