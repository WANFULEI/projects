#pragma once

#include <QThread>

class QGLWidget;
class swapbuffer_thread :
	public QThread
{
public:
	swapbuffer_thread(QGLWidget *gl_widget);
	~swapbuffer_thread(void);

private:
	void run();

private:
	QGLWidget *m_gl_widget;
};

