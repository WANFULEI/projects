#ifndef TT4_H
#define TT4_H

#include <QtWidgets/QMainWindow>
#include "ui_tt4.h"
#include "mywidget.h"
#include "../layerwidget/Qt_widget2.h"

class tt4 : public QMainWindow
{
	Q_OBJECT

public:
	tt4(QWidget *parent = 0);
	~tt4();

protected slots:
	void on_actionPan_triggered();
	void on_actionZoomIn_triggered();

private:
	Ui::tt4Class ui;
	Qt_widget2 * m_pMyWidget;
};

#endif // TT4_H
