#include <gl\glew.h>
#include "layer_widget.h"
#include <qdebug>
#include <assert.h>

namespace layer_wgt{


layer_widget::layer_widget()
{
	active_layer = -1;
}

layer_widget::~layer_widget()
{

}

#define TRANSLATE_EVENT(func, type)							\
layer_widget::##func##(##type## *e){							\
	if (active_layer == -1)									\
	{														\
		return;												\
	}														\
	if (active_layer < 0 || active_layer >= layers.count() )		\
	{														\
		active_layer = -1;									\
	}														\
	auto layer = layers.get_entry(active_layer);				\
	if (layer == 0)											\
	{														\
		return;												\
	}														\
	return layer->##func##(e);									\
}

void TRANSLATE_EVENT(mousePressEvent,QMouseEvent)
void TRANSLATE_EVENT(mouseReleaseEvent,QMouseEvent)
void TRANSLATE_EVENT(mouseMoveEvent,QMouseEvent)
void TRANSLATE_EVENT(mouseDoubleClickEvent,QMouseEvent)
void TRANSLATE_EVENT(wheelEvent,QWheelEvent)
void TRANSLATE_EVENT(keyPressEvent,QKeyEvent)
void TRANSLATE_EVENT(keyReleaseEvent,QKeyEvent)
void TRANSLATE_EVENT(enterEvent,QEvent)
void TRANSLATE_EVENT(leaveEvent,QEvent)

void layer_widget::initializeGL()
{
// 	QGLFormat ft = format();
// 	ft.setDoubleBuffer(true);
// 	setFormat(ft);
	bool res = format().doubleBuffer();
	assert(res);

	glewInit();

	glClearColor(0.0, 0.0, 0.0, 0.0);

	glShadeModel(GL_FLAT);

	glEnable(GL_LINE_SMOOTH);

	glEnable(GL_DEPTH_TEST);

	glDepthFunc(GL_LEQUAL);

	glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);

	//启用顶点数组
	glEnableClientState(GL_VERTEX_ARRAY);

	glEnable(GL_BLEND); 

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_LINE_SMOOTH); 

	glEnable(GL_POINT_SMOOTH);

	glClearColor( 0.0, 0.0, 0.0, 0.0 );  
	glColor3f( 1.0, 1.0, 1.0 );  

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


	//返回当前OpenGL实现的版本号
	printf("GL_VERSION:%s\n", glGetString(GL_VERSION));

	printf("GL_VENDOR:%s\n", glGetString(GL_VENDOR));

	printf("GL_RENDERER:%s\n", glGetString(GL_RENDERER));

	printf("GLU_VERSION:%s\n", glGetString(GLU_VERSION));

	printf("GLU_EXTENSIONS:%s\n", glGetString(GLU_EXTENSIONS));

	printf("GL_SHADING_LANGUAGE_VERSION:%s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	GLint nMaxStack = 0;
	glGetIntegerv(GL_MAX_MODELVIEW_STACK_DEPTH,&nMaxStack);
	printf("GL_MAX_MODELVIEW_STACK_DEPTH:%d\n", nMaxStack);

	if ( !GLEW_ARB_vertex_program )
	{
		fprintf(stderr, "ARB_vertex_programis missing!\n");
	}
	//获得OpenGL扩展信息s

	GLint nExtensions;
	glGetIntegerv(GL_NUM_EXTENSIONS, &nExtensions);
	printf("GL_NUM_EXTENSIONS:%d\n", nExtensions);
}

void layer_widget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	glClearColor(0.0, 0.0, 0.0, 1.0);

// 	glMatrixMode(GL_PROJECTION);
// 	glLoadIdentity();
// 
// 	glMatrixMode(GL_MODELVIEW);
// 	glLoadIdentity();

	for (int i=0;i<layers.size();++i)
	{
		layers[i]->gl_draw();
	}

//	swapBuffers();
}

void layer_widget::resizeGL( int width, int height )
{
 	glViewport(0, 0, (GLint)width, (GLint)height);  

	glMatrixMode( GL_PROJECTION );  
	glLoadIdentity();
  
	//定义剪裁面  
	rebuild_coor(); 

}

void layer_widget::attach(layer *l)
{
	if (l == 0)
	{
		return;
	}
	l->widget = this;
	layers.attach_entry(l);
}

void layer_widget::set_active_layer(layer *l)
{
	active_layer = get_layer_index(l);
}

int layer_widget::get_layer_index(layer *l)
{
	return layers.indexOf(l,0);
}

}