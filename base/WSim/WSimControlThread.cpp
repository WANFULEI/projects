#include "WSimControlThread.h"
#include "WSimObject.h"


void WSimControlThread::run()
{
	m_currentTime = 0;
	while(isStart()){
		usleep(1 / m_updateTimeRate * 1e6);
		m_currentTime += 1 / m_updateTimeRate * 1e6 * m_speed;
	}
	m_currentTime = 0;
}

WSimControlThread * WSimControlThread::instance()
{
	static WSimControlThread __instance;
	return &__instance;
}

void WSimControlThread::start(QThread::Priority pri /*= InheritPriority */)
{
	WThread::start(pri);
	for(auto iter = m_simThreads.begin(); iter != m_simThreads.end(); ++iter){
		(*iter)->start(pri);
	}
}

void WSimControlThread::stop()
{
	WThread::stop();
	for(auto iter = m_simThreads.begin(); iter != m_simThreads.end(); ++iter){
		(*iter)->stop();
	}
}

WSimControlThread::WSimControlThread(void)
{
	m_currentTime = 0;
	m_updateTimeRate = 30;
	m_speed = 1;
	m_update2DThread = new WSimThread;
	m_update2DThread->setType(WSimThread::update2D);
	m_simThreads << m_update2DThread;
	m_update3DThread = new WSimThread;
	m_update3DThread->setType(WSimThread::update3D);
	m_simThreads << m_update3DThread;
}

WSimControlThread::~WSimControlThread(void)
{
	for(auto iter = m_simThreads.begin(); iter != m_simThreads.end(); ++iter){
		delete *iter;
	}
}

WSimObject * WSimControlThread::getSimObject(QString name) const
{
	for(auto iter=m_simThreads.begin(); iter != m_simThreads.end(); ++iter){
		QList<WSimObject *> objs = (*iter)->getSimObjects();
		for(auto iter2 = objs.begin(); iter2 != objs.end(); ++iter2){
			if((*iter2)->getName() == name) return *iter2;
		}
	}
	return 0;
}

QList<WSimObject *> WSimControlThread::getSimObjects(QString name) const
{
	QList<WSimObject *> res;
	for(auto iter=m_simThreads.begin(); iter != m_simThreads.end(); ++iter){
		QList<WSimObject *> objs = (*iter)->getSimObjects();
		for(auto iter2 = objs.begin(); iter2 != objs.end(); ++iter2){
			if((*iter2)->getName() == name) res << *iter2;
		}
	}
	return res;
}
