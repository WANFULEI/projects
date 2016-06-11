#pragma once

#include "../../control/layerwidget/Qt_widget_layer.h"
#include <QRect>
#include <QRectF>
#include <QImage>
#include <QReadWriteLock>
#include <QThread>
#include <QRunnable>

class GDALDataset;
class raster_layer : public layerwidget::Qt_widget_layer
{
public:
	raster_layer(void);
	~raster_layer(void);

	bool load(const QString & url);

protected:
	virtual void draw();
	QRect calc_tif_rect(const QRectF & geo_rect);
	QImage get_image(const QRect & rect,int factor = 1);
	QRect calc_back_rect(const QRect & rect);
	void clip_tif_rect( QRect & rect );

	
	class back_image_thread : public QThread
	{
		//Q_OBJECT
	public:
		back_image_thread(raster_layer * layer);

		void start(Priority = InheritPriority);
		void stop();

	protected:
		void run();
	private:
		raster_layer * m_raster_layer;
		volatile bool m_run;

// 	private slots:
// 		void slot_finished();
	};
	friend class back_image_thread;

	class update_rect : public QRunnable
	{
	public:
		update_rect(raster_layer * layer,const QRect & tif_rect);

	protected:
		void run();

	private:
		raster_layer * m_raster_layer;
		QRect m_tif_rect;
	};

private:
	QString m_url;
	GDALDataset * m_data;
	QImage m_back_image;
	QRect m_back_rect;
	QReadWriteLock m_back_lock;
	volatile bool m_is_backed;
	back_image_thread * m_back_image_thread;
};

