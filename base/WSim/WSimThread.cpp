#include "WSimThread.h"
#include "osg\Timer"
#include "WSimObject.h"

void WSimThread::run()
{
	while(isStart()){
		double minFrameTime = m_runMaxFrameRate>0.0 ? 1.0/m_runMaxFrameRate : 0.0;
		osg::Timer_t startFrameTick = osg::Timer::instance()->tick();

		if(m_type == update){
			for(auto iter = m_simObjects.begin(); iter != m_simObjects.end(); ++iter){
				(*iter)->update();
			}
		}else if(m_type == update2D){
			for(auto iter = m_simObjects.begin(); iter != m_simObjects.end(); ++iter){
				(*iter)->update2D();
			}
		}else if(m_type == update3D){
			for(auto iter = m_simObjects.begin(); iter != m_simObjects.end(); ++iter){
				(*iter)->update3D();
			}
		}

		// work out if we need to force a sleep to hold back the frame rate
		osg::Timer_t endFrameTick = osg::Timer::instance()->tick();
		double frameTime = osg::Timer::instance()->delta_s(startFrameTick, endFrameTick);
		if (frameTime < minFrameTime) usleep(static_cast<unsigned int>(1000000.0*(minFrameTime-frameTime)));
	}
}
