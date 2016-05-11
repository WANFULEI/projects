#ifndef TEST_DEMO_CORE2_H
#define TEST_DEMO_CORE2_H

#include "test_demo_core2_global.h"
#include "../../framecore/framecore/framecore.h"

class TEST_DEMO_CORE2_EXPORT test_demo_core2 : public framecore::component
{
public:
	test_demo_core2();
	~test_demo_core2();

	virtual void action_triggered(QAction * action);

	virtual QWidget * create_control(const QString & toolbar_id,const QString & control_id);

private:

};

class TEST_DEMO_CORE2_EXPORT test_demo_core3 : public framecore::component
{
public:
	test_demo_core3();
	~test_demo_core3();

	virtual void action_triggered(QAction * action);

	

private:

};

extern "C"
{
	TEST_DEMO_CORE2_EXPORT framecore::component * create_component(const QString & class_name);
};

#endif // TEST_DEMO_CORE2_H
