#pragma once

#include "../../control/layerwidget/Qt_widget_layer.h"
#include <QRect>
#include <QRectF>
#include <QImage>

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
	QImage get_image(const QRect & rect);

private:
	QString m_url;
	GDALDataset * m_data;
};

