#include "swapbuffer_thread.h"
#include "QGLWidget"


swapbuffer_thread::swapbuffer_thread(QGLWidget *gl_widget)
{
	m_gl_widget = gl_widget;
}


swapbuffer_thread::~swapbuffer_thread(void)
{
}

void swapbuffer_thread::run()
{
// 	while (1)
// 	{
// 		m_gl_widget->makeCurrent();
// 		m_gl_widget->swapBuffers();
// 		m_gl_widget->doneCurrent();
// 	}
}
