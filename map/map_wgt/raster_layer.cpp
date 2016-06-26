#include <gl\glew.h>
#include "raster_layer.h"
#include "gdal_priv.h"
#include "cpl_conv.h" // for CPLMalloc()

#include <QColor>
#include "..\..\control\layer_widget\layer_widget.h"
#include <windows.h>
#include <QTime>
#include <QDebug>
#include "QLibrary"
#include "..\..\template\time_elapsed.h"

namespace map_wgt{

raster_layer::raster_layer(void)
{
	m_data = 0;
}


raster_layer::~raster_layer(void)
{
	if (m_data)
	{
		GDALClose((GDALDatasetH)m_data);
	}
}

void raster_layer::gl_draw()
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

	//qDebug() << QObject::tr("x1:%1, x2:%2").arg(x1).arg(x2);
	if (x2-x1-1 < 0 || y2-y1-1 < 0)
	{
		return;
	}
	QImage image = get_image(calc_tif_rect(rect), x2-x1-1, y2-y1-1);
	if (image.isNull())
	{
		return;
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT,4);
	glRasterPos2d(widget->x_real(x1+1), widget->y_real(y1+1));
	
	//glWindowPos3i(x1, widget->height()-y1, 0);
	glPixelZoom(1.0,-1.0);  //从上到下绘制
	time_elapsed timer;
	glDrawPixels(image.width(), image.height(), GL_RGBA, GL_UNSIGNED_BYTE,image.bits());
	qDebug() << "glDrawPixels:" << timer.stop() / 1000;

	delete [] (char *)image.bits();
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

	if (offx < 0)
	{
		offx = 0;
	}
	if (offy < 0)
	{
		offy = 0;
	}
	if (sizex > m_data->GetRasterXSize())
	{
		sizex = m_data->GetRasterXSize();
	}
	if (sizey > m_data->GetRasterYSize())
	{
		sizey = m_data->GetRasterYSize();
	}

	res.setLeft(offx);
	res.setTop(offy);
	res.setSize(QSize(sizex,sizey));
	res = res.normalized();
	return res;
}

void myImageCleanupHandler(void *info)
{
	char * datas = (char *)info;
	delete [] datas;
}

QImage raster_layer::get_image(const QRect & rect, int width, int height)
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

	int scr_width = width;
	int scr_height = height;
	int scr_area = scr_width * scr_height;
	int image_x = rect.left();
	int image_y = rect.top();
	int image_width = rect.width();
	int image_height = rect.height();
	
	char * datas = new char[ scr_area * 4 ];
	memset(datas, 255, scr_area * 4);

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
			4, 4*scr_width );
	}

// 	m_data->RasterIO( GF_Read, image_x, image_y, image_width, image_height, 
// 		datas, scr_width, scr_height, GDT_Byte, 
// 		3, 0, 3, 3*scr_width, 1 );
	qDebug() << "RasterIO:" << timer.elapsed();

	res = QImage( (uchar *)datas, scr_width, scr_height,scr_width*4, QImage::Format_ARGB32 );
	return res;
}

}