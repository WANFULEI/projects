#ifndef TT4_H
#define TT4_H

#include <QtGui/QMainWindow>
#include "ui_tt4.h"
#include "../layerwidget/Qt_default_widget.h"

class tt4 : public QMainWindow
{
	Q_OBJECT

public:
	tt4(QWidget *parent = 0);
	~tt4();

protected slots:
	void on_actionPan_triggered();
	void on_actionSelect_triggered();

private:
	Ui::tt4Class ui;
	Qt_default_widget * m_pMyWidget;
};

#endif // TT4_H
