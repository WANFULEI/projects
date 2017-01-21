#include "IconDlg.h"
#include "QBoxLayout"
#include "QDir"
#include "QTableWidget"
#include "QHeaderView"
#include "QtSvg\QSvgRenderer"
#include "QPainter"

IconDlg::IconDlg(QWidget *parent)
	:QDialog(parent)
{
	QHBoxLayout *layout = new QHBoxLayout;
	m_tab = new QTabWidget;
	layout->addWidget(m_tab);
	setLayout(layout);
}


IconDlg::~IconDlg(void)
{

}

void IconDlg::init(QString path)
{
#define COLUMN_COUNT 6
	if(path.isEmpty()) return;
	QDir dir(path);
	QStringList dirs = dir.entryList(QDir::Dirs, QDir::Name);
	int maxTableWidth = 0;
	int maxRow = 0;
	for(int i=0; i<dirs.size(); ++i){
		if(dirs[i] == "." || dirs[i] == "..") continue;
		QTableWidget *table = new QTableWidget;
		table->horizontalHeader()->hide();
		table->verticalHeader()->hide();
		m_tab->addTab(table, dirs[i]);
		dir.setPath(path + "/" + dirs[i]);
		QStringList files = dir.entryList(QStringList(), QDir::Files, QDir::Name);
		table->setColumnCount(COLUMN_COUNT);
		int row = 0, column = 0;
		int rowCount = files.size() / COLUMN_COUNT + ((files.size() % COLUMN_COUNT > 0) > 0 ? 1 : 0);
		table->setRowCount(rowCount);
		for(auto j=files.begin(); j!=files.end(); ++j){
			if(column == COLUMN_COUNT){
				row++;
				column = 0;
			}
			auto item = new QTableWidgetItem(QIcon(dir.absolutePath() + "/" + *j), QFileInfo(*j).baseName());
			item->setFlags(item->flags() & ~Qt::ItemIsEditable);
			item->setToolTip(QFileInfo(*j).fileName());
			item->setData(Qt::UserRole, dir.absolutePath() + "/" + *j);
			table->setItem(row, column++, item);
		}
		int tableWidth = 0;
		QFontMetrics fm(table->font());
		for(int j=0; j<COLUMN_COUNT; ++j){
			int maxWidth = 0;
			for(int k=0; k<rowCount; ++k){
				if(table->item(k, j) == 0) continue;
				if(maxWidth < fm.width(table->item(k, j)->text()))
					maxWidth = fm.width(table->item(k, j)->text());
			}
			table->setColumnWidth(j, maxWidth + 32);
			tableWidth += maxWidth + 32;
		}
		if (tableWidth > maxTableWidth){
			maxTableWidth = tableWidth;
		}
		if(rowCount > maxRow) maxRow = rowCount;
		connect(table, SIGNAL(cellClicked(int, int)), this, SLOT(slotCellClicked(int, int)));
	}
	resize(maxTableWidth, 120/*maxRow * 24 + 60*/);
}

void IconDlg::slotCellClicked(int row, int column)
{
	QTableWidget *table = dynamic_cast<QTableWidget *>(sender());
	QTableWidgetItem *item = table->item(row, column);
	if(item == 0) return;
	emit signSelect(item->data(Qt::UserRole).toString());
}
