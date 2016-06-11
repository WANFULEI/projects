#include "raster_layer.h"
#include "gdal_priv.h"
#include "cpl_conv.h" // for CPLMalloc()

#include <QColor>
#include "..\..\control\layerwidget\Qt_widget.h"
#include <windows.h>
#include <QTime>
#include <QDebug>
#include "QThreadPool"

raster_layer::raster_layer(void)
	:m_back_lock(QReadWriteLock::Recursive)
{
	m_data = 0;	
	m_is_backed = false;
	m_back_image_thread = new back_image_thread(this);
	m_back_image_thread->setParent(this);
	m_back_image_thread->start();
}


raster_layer::~raster_layer(void)
{
	if (m_data)
	{
		GDALClose((GDALDatasetH)m_data);
	}
	m_back_image_thread->stop();
}

void raster_layer::draw()
{
	if (widget == 0)
	{
		return;
	}

	// makesure geo rect
	double x_min = widget->x_min();
	if (x_min < -180)
	{
		x_min = -180;
	}
	double x_max = widget->x_max();
	if (x_max > 180)
	{
		x_max = 180;
	}
	double y_min = widget->y_min();
	if (y_min < -90)
	{
		y_min = -90;
	}
	double y_max = widget->y_max();
	if (y_max > 90)
	{
		y_max = 90;
	}

	// makesure screen pos
	int x1 = widget->x_pixel(x_min);
	int x2 = widget->x_pixel(x_max);
	int y1 = widget->y_pixel(y_max);
	int y2 = widget->y_pixel(y_min);

	// makesure layer in screen, if not, return
	QRect rc = widget->rect();
	if (!rc.contains(QPoint(x1,y1)) && !rc.contains(x2,y2))
	{
		return;
	}

	QRectF rect;
	rect.setLeft(x_min);
	rect.setRight(x_max);
	rect.setTop(y_max);
	rect.setBottom(y_min);

	QRect tif_rect = calc_tif_rect(rect);

	m_back_lock.lockForRead();
	if (m_back_image.isNull()) // no back image, only calc, get, and draw
	{
		m_back_lock.unlock();

		QImage image = get_image(tif_rect);
		if (image.isNull())
		{
			return;
		}

		widget->get_painter().drawImage(QRect(x1,y1,x2-x1,y2-y1),image);

		delete [] (char *)image.bits();

// 		m_back_lock.lockForWrite();
// 		m_back_rect = calc_back_rect(tif_rect);
// 		m_is_backed = false;
// 		m_back_lock.unlock();
	}
	else
	{
		if (m_back_rect.contains(tif_rect)) // back image contains update rect, use back image
		{
			widget->get_painter().drawImage(QRect(x1,y1,x2-x1,y2-y1),m_back_image,
				QRect( (tif_rect.left()-m_back_rect.left())*1.0/m_back_rect.width()*m_back_image.width(), 
				(tif_rect.top()-m_back_rect.top())*1.0/m_back_rect.height()*m_back_image.height(), 
				x2-x1,y2-y1 ) );

			//m_back_image.save("f:\\1.png");

// 			qDebug() << "dst rect:" << QRect(x1,y1,x2-x1,y2-y1);
// 			qDebug() << "src rect:" << QRect( (tif_rect.left()-m_back_rect.left())/m_back_rect.width()*m_back_image.width(), 
// 				(tif_rect.top()-m_back_rect.top())/m_back_rect.height()*m_back_image.height(), 
// 				x2-x1,y2-y1 );
			m_back_lock.unlock();
		}
		else // not contain, only calc, get, and draw
		{
			m_back_lock.unlock();

			QImage image = get_image(tif_rect);
			if (image.isNull())
			{
				return;
			}

			widget->get_painter().drawImage(QRect(x1,y1,x2-x1,y2-y1),image);

			delete [] (char *)image.bits();



		}
	}

	QThreadPool::globalInstance()->start(new update_rect(this,tif_rect));
}

bool raster_layer::load(const QString & url)
{
	static bool load_driver = false;
	if (!load_driver)
	{
		load_driver = true;
		GDALAllRegister();
	}

	m_data = (GDALDataset *) GDALOpen( url.toStdString().c_str() , GA_ReadOnly );
	m_url = url;
	return m_data != 0;
}

QRect raster_layer::calc_tif_rect(const QRectF & geo_rect)
{
	QRect res;
	if (m_data == 0)
	{
		return res;
	}
	QRectF param(geo_rect);

	if (geo_rect.left() > geo_rect.right())
	{
		double tmp = geo_rect.left();
		param.setLeft(geo_rect.right());
		param.setRight(tmp);
	}
	if (geo_rect.top() < geo_rect.bottom())
	{
		double tmp = geo_rect.top();
		param.setTop(geo_rect.bottom());
		param.setBottom(tmp);
	}

	double adfGeoTransform[6];
	if( m_data->GetGeoTransform( adfGeoTransform ) != CE_None )
	{
		return res;
	}
	
	int offx = (param.left() - adfGeoTransform[0]) / adfGeoTransform[1];
	int offy = (param.top() - adfGeoTransform[3]) / adfGeoTransform[5];
	int sizex = (param.right() - param.left()) / adfGeoTransform[1];
	int sizey = (param.bottom() - param.top()) / adfGeoTransform[5];

	res.setLeft(offx);
	res.setTop(offy);
	res.setSize(QSize(sizex,sizey));
	clip_tif_rect(res);
	return res;
}

void myImageCleanupHandler(void *info)
{
	char * datas = (char *)info;
	delete [] datas;
}

QImage raster_layer::get_image(const QRect & rect,int factor)
{
	QImage res;
	if (!rect.isValid())
	{
		return res;
	}
	if (m_data == 0)
	{
		return res;
	}

	int count = m_data->GetRasterCount();
	if (count != 3)
	{
		return res;
	}

	int scr_width = widget->width() * factor;
	int scr_height = widget->height() * factor;
	int scr_area = scr_width * scr_height;
	int image_x = rect.left();
	int image_y = rect.top();
	int image_width = rect.width();
	int image_height = rect.height();
	
	char * datas = new char[ scr_area * 3 ];

	QTime timer = QTime::currentTime();
	for (int i=0;i<3;++i)
	{
		GDALRasterBand * band = m_data->GetRasterBand(i + 1);
		if (band == 0)
		{
			delete [] datas;
			return res;
		}
		band->RasterIO( GF_Read, image_x, image_y, image_width, image_height, 
			datas+i, scr_width, scr_height, GDT_Byte, 
			3, 3*scr_width );
	}

// 	m_data->RasterIO( GF_Read, image_x, image_y, image_width, image_height, 
// 		datas, scr_width, scr_height, GDT_Byte, 
// 		3, 0, 3, 3*scr_width, 1 );
	qDebug() << "RasterIO:" << timer.elapsed();

	res = QImage( (uchar *)datas, scr_width, scr_height,scr_width*3, QImage::Format_RGB888 );
	return res;
}

QRect raster_layer::calc_back_rect(const QRect & rect)
{
	if (!rect.isValid())
	{
		return rect;
	}
	QRect tmp = QRect( rect.left()-rect.width(), rect.top()-rect.height(), rect.width()*3, rect.height()*3 );
	clip_tif_rect(tmp);
	return tmp;
}

void raster_layer::clip_tif_rect(QRect & rect)
{
	if (m_data == 0)
	{
		return;
	}
	rect = rect.normalized();
	QRect tmp( 0, 0, m_data->GetRasterXSize(), m_data->GetRasterYSize() );
	rect = tmp.intersect(rect);
}

raster_layer::back_image_thread::back_image_thread(raster_layer * layer)
{
	m_raster_layer = layer;
	m_run = false;
}

void raster_layer::back_image_thread::run()
{
	while (m_run)
	{
		m_raster_layer->m_back_lock.lockForRead();
		QRect rect = m_raster_layer->m_back_rect;
		if(!rect.isValid() || m_raster_layer->m_is_backed)
		{
			m_raster_layer->m_back_lock.unlock();
			msleep(20);
			continue;
		}
		m_raster_layer->m_back_lock.unlock();
		
		QImage image = m_raster_layer->get_image(rect,3);

		m_raster_layer->m_back_lock.lockForRead();
		if (!m_raster_layer->m_back_image.isNull())
		{
			//delete [] (char *)m_raster_layer->m_back_image.bits();
		}
		m_raster_layer->m_back_image = image;
		m_raster_layer->m_is_backed = true;
		m_raster_layer->m_back_lock.unlock();
		msleep(20);
	}
}

void raster_layer::back_image_thread::start(Priority pri /*= InheritPriority*/)
{
	m_run = true;
	connect(this,SIGNAL(finished()),this,SLOT(slot_finished()));
	QThread::start(pri);
}

void raster_layer::back_image_thread::stop()
{
	m_run = false;
}

// void raster_layer::back_image_thread::slot_finished()
// {
// 
// }

raster_layer::update_rect::update_rect(raster_layer * layer,const QRect & tif_rect)
{
	m_raster_layer =layer;
	m_tif_rect = tif_rect;
}

void raster_layer::update_rect::run()
{
	m_raster_layer->m_back_lock.lockForWrite();
	m_raster_layer->m_back_rect = m_raster_layer->calc_back_rect(m_tif_rect);
	m_raster_layer->m_is_backed = false;
	m_raster_layer->m_back_lock.unlock();
}
