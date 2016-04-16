#include "test_demo_core.h"

test_demo_core::test_demo_core(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	initialize();
}

test_demo_core::~test_demo_core()
{

}

bool test_demo_core::load_from_xml(TiXmlElement * pXmlNode)
{
	return demo_core::application::load_from_xml(pXmlNode);
}

bool test_demo_core::initialize()
{
	TiXmlDocument doc;
	if (!doc.LoadFile((qApp->applicationDirPath() + "/config/application.cfg").toStdString().c_str()))
	{
		return false;
	}
	return load_from_xml(doc.RootElement());
}
