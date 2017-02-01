#pragma once
#include "QDialog"
#include "QTabWidget"

class IconDlg :
	public QDialog
{
	Q_OBJECT
public:
	IconDlg(QWidget *parent = 0);
	~IconDlg(void);

	void init(QString path);

signals:
	void signSelect(QString icon);

public slots:
	void slotCellClicked ( int row, int column );

private:
	QTabWidget *m_tab;
};

