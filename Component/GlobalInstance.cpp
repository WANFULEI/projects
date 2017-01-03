#include "GlobalInstance.h"


GlobalInstance::GlobalInstance(void)
{
}


GlobalInstance::~GlobalInstance(void)
{
}

GlobalInstance *GlobalInstance::getInstance(){
	static GlobalInstance instance;
	return &instance;
}
