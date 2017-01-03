#pragma once
#include <QObject>
#include "component_global.h"

class COMPONENT_EXPORT GlobalInstance
{
public:
	GlobalInstance(void);
	~GlobalInstance(void);

	static GlobalInstance *getInstance();
	QObject *getMainWindow() const { return m_mainWindow; }
	void setMainWindow(QObject *mainWindow) { m_mainWindow = mainWindow; }

private:
	QObject *m_mainWindow;
};

