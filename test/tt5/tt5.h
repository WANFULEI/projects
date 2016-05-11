#ifndef TT5_H
#define TT5_H

#include <QtGui/QMainWindow>
#include "ui_tt5.h"

class tt5 : public QMainWindow
{
	Q_OBJECT

public:
	tt5(QWidget *parent = 0, Qt::WFlags flags = 0);
	~tt5();

private:
	Ui::tt5Class ui;
};

#endif // TT5_H
