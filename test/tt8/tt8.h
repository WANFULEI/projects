#ifndef TT8_H
#define TT8_H

#include <QtGui/QWidget>
#include "ui_tt8.h"

class tt8 : public QWidget
{
	Q_OBJECT

public:
	tt8(QWidget *parent = 0, Qt::WFlags flags = 0);
	~tt8();

private:
	Ui::tt8Class ui;
};

#endif // TT8_H
