#pragma once
#include "WSimThread.h"
#include "wsim_global.h"

class WSIM_EXPORT WSimControlThread :
	public WThread
{
public:

	static WSimControlThread *instance();

	void start(QThread::Priority pri = InheritPriority );
	void stop();

	double getNowTime() const { return m_currentTime; }

	void setUpdateTimeRate(double frameRate) { m_updateTimeRate = frameRate; }
	double getUpdateTimeRate() const { return m_updateTimeRate; }

	void setSpeed(double speed) { m_speed = speed; }
	double getSpeed() const { return m_speed; }
	void speedUp() { m_speed *= 2; }
	void slowDown() { m_speed /= 2; }

	void addThread(WSimThread *thread) { if(thread && !m_simThreads.contains(thread)) m_simThreads << thread; }
	void removeThread(WSimThread *thread) { m_simThreads.removeOne(thread); }

	WSimObject *getSimObject(QString name) const;
	QList<WSimObject *> getSimObjects(QString name) const;

	void addSimObject(WSimObject *obj) { m_update2DThread->addSimObject(obj); m_update3DThread->addSimObject(obj); }
	void removeSimObject(WSimObject *obj) { m_update2DThread->removeSimObject(obj); m_update3DThread->removeSimObject(obj); }

	WSimThread *get2DThread() const { return m_update2DThread; }
	WSimThread *get3DThread() const { return m_update3DThread; }

protected:
	virtual void run();

private:
	WSimControlThread(void);
	~WSimControlThread(void);

private:
	double m_currentTime;
	double m_updateTimeRate;
	double m_speed;
	QList<WSimThread *> m_simThreads;
	WSimThread *m_update2DThread;
	WSimThread *m_update3DThread;
};

