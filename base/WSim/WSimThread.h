#pragma once
#include "QThread"
#include "wsim_global.h"

class WSIM_EXPORT WThread : public QThread
{
public:
	WThread(){
		m_start = false;
	}

	void start(QThread::Priority pri = InheritPriority) { m_start = true; QThread::start(pri); }
	bool isStart() const { return m_start; }
	void stop() { m_start = false; wait(); }

private:
	volatile bool m_start;
};

class WSimObject;
class WSIM_EXPORT WSimThread :
	public WThread
{
public:
	enum Type{
		update2D,
		update3D,
		update
	};
	WSimThread(void){
		m_runMaxFrameRate = 0;
		m_type = update;
	}
	~WSimThread(void){}

	void setRunMaxFrameRate(double frameRate) { m_runMaxFrameRate = frameRate; }
	double getRunMaxFrameRate() const { return m_runMaxFrameRate; }

	void setType(Type type) { m_type = type; }
	Type getType() const { return m_type; }

	void addSimObject(WSimObject *obj) { if(obj && !m_simObjects.contains(obj)) m_simObjects << obj; }
	void removeSimObject(WSimObject *obj) { m_simObjects.removeOne(obj); }

	QList<WSimObject *> getSimObjects() const { return m_simObjects; }

protected:
	virtual void run();

private:
	QList<WSimObject *> m_simObjects;
	double m_runMaxFrameRate;
	Type m_type;
};

