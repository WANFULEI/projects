#ifndef TEST_DEMO_CORE_H
#define TEST_DEMO_CORE_H

#include <QtWidgets/QMainWindow>
#include "ui_test_demo_core.h"
#include "../demo_core/demo_core.h"

class test_demo_core : public QMainWindow , public demo_core::application
{
	Q_OBJECT

public:
	test_demo_core(QWidget *parent = 0);
	~test_demo_core();

	bool initialize();
	virtual bool load_from_xml(TiXmlElement * pXmlNode);

private:
	Ui::test_demo_coreClass ui;
};

#endif // TEST_DEMO_CORE_H
