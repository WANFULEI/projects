#ifndef TT4_H
#define TT4_H

#include <QtWidgets/QMainWindow>
#include "ui_tt4.h"
#include "mywidget.h"

class tt4 : public QMainWindow
{
	Q_OBJECT

public:
	tt4(QWidget *parent = 0);
	~tt4();

	mywidget * MyWidget() const { return m_pMyWidget; }
	void MyWidget(mywidget * val) { m_pMyWidget = val; }

private:
	Ui::tt4Class ui;
	mywidget * m_pMyWidget;
};

#endif // TT4_H
