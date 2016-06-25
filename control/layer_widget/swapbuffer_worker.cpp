#include "swapbuffer_worker.h"
#include "layer_widget.h"

namespace layer_wgt{

void swapbuffer_worker::run()
{
	m_gl->makeCurrent();
	m_gl->swapBuffers();
	m_gl->doneCurrent();
	m_gl->swapbuffer_down = true;
}

}