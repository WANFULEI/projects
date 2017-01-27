#pragma once
#include "drawtool.h"
#include "IconDlg.h"
#include "QObject"
#include "osg\Image"

class DrawKeypointTool : public QObject,
	public DrawTool
{
	Q_OBJECT
public:
	DrawKeypointTool(QString path, QWidget *parent = 0);
	~DrawKeypointTool(void);

protected:
	virtual void canvasReleaseEvent(QgsMapMouseEvent* e);
	void onActive() { m_dlg->show(); }
	void onDeActive() { m_dlg->hide(); }

private slots:
	void slotSelect(QString path);
private:
	IconDlg *m_dlg;
	QString m_path;
	int m_iconWidth;
	int m_iconHeight;
	QImage m_cache;
	osg::ref_ptr<osg::Image> m_image;
};

