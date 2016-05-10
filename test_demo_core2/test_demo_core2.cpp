#include "test_demo_core2.h"
#include <QtGui/QMessageBox>
#include <QCheckBox>

test_demo_core2::test_demo_core2()
{

}

test_demo_core2::~test_demo_core2()
{

}



test_demo_core3::test_demo_core3()
{

}

test_demo_core3::~test_demo_core3()
{

}

void test_demo_core3::action_triggered(QAction * action)
{
	if (action == 0)
	{
		return;
	}

	QMessageBox::warning(0,"in test_demo_core3",action->data().toHash()["action_id"].toString());
}

void test_demo_core2::action_triggered(QAction * action)
{
	if (action == 0)
	{
		return;
	}

	QMessageBox::warning(0,"in test_demo_core2",action->data().toHash()["action_id"].toString());
}

QWidget * test_demo_core2::create_control(const QString & toolbar_id,const QString & control_id)
{
	return new QCheckBox("widget_test");
}

TEST_DEMO_CORE2_EXPORT demo_core::component * create_component(const QString & class_name)
{
	if (class_name == "test_demo_core2")
	{
		return new test_demo_core2;
	}
	else if (class_name == "test_demo_core3")
	{
		return new test_demo_core3;
	}
	else{
		return 0;
	}
	
}
