#include "test_demo_core2.h"
#include <QtGui/QMessageBox>
#include <QCheckBox>
#include "../../framecore/framecore/file.h"
#include "../../framecore/framecore/file_type.h"
#include "../../framecore/framecore/file_manager.h"
using namespace framecore;

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

class test_file : public file
{
public:


private:
	int m_data;
};

void test_demo_core2::action_triggered(QAction * action)
{
	if (action == 0)
	{
		return;
	}

	file_type file_type;
	file_type.set_ext(".test_file");
	file_manager::get_instance()->register_file_type(file_type);
	test_file * file = new test_file;
	file->set_file_type(file_type);
	file_manager::get_instance()->attach_file(file);

	auto file2 = file_manager::get_instance()->create_file<test_file>(file_type);
	file2->set_path("");

	QMessageBox::warning(0,"in test_demo_core2",action->data().toHash()["action_id"].toString());
}

QWidget * test_demo_core2::create_control(const QString & toolbar_id,const QString & control_id)
{
	return new QCheckBox("widget_test");
}

TEST_DEMO_CORE2_EXPORT framecore::component * create_component(const QString & class_name)
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
