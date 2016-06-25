#pragma once

#include <QRunnable>

namespace layer_wgt{
class layer_widget;
class swapbuffer_worker : public QRunnable
{
public:
	swapbuffer_worker(layer_widget *gl){
		m_gl = gl;
	}
protected:
	void run();

private:
	layer_widget *m_gl;
};

}
