#pragma once
#include "QString"
#include "wsim_global.h"
class WSIM_EXPORT WSimObject
{
public:
	WSimObject(QString name = "");
	~WSimObject(void);

	virtual void update(){}
	virtual void update2D(){}
	virtual void update3D(){}

	void setName(QString name) { m_name = name; }
	QString getName() const { return m_name; }

private:
	QString m_name;
};

