#ifndef TEST_DEMO_CORE2_H
#define TEST_DEMO_CORE2_H

#include "test_demo_core2_global.h"
#include "../demo_core/demo_core.h"

class TEST_DEMO_CORE2_EXPORT test_demo_core2 : public demo_core::component
{
public:
	test_demo_core2();
	~test_demo_core2();

private:

};

class TEST_DEMO_CORE2_EXPORT test_demo_core3 : public demo_core::component
{
public:
	test_demo_core3();
	~test_demo_core3();

private:

};

extern "C"
{
	TEST_DEMO_CORE2_EXPORT demo_core::component * create_component(const QString & class_name);
};

#endif // TEST_DEMO_CORE2_H
