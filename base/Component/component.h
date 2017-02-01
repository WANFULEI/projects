#ifndef COMPONENT_H
#define COMPONENT_H

#include "component_global.h"
#include <QObject>
#include <qplugin.h>

class COMPONENT_EXPORT Component : public QObject
{
	Q_OBJECT
public:
	Component();
	virtual ~Component();

	virtual void initialize();

private:
	QString m_name;
	QString m_library;
	QString m_className;
};

#endif // COMPONENT_H
