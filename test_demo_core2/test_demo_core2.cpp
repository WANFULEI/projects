#include "test_demo_core2.h"

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
