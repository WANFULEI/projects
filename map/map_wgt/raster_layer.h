#pragma once

#include <QRect>
#include <QRectF>
#include <QImage>
#include "../../control/layer_widget/layer.h"
#include "map_wgt_global.h"

class GDALDataset;

namespace map_wgt{

class MAP_WGT_EXPORT raster_layer : public layer_wgt::layer
{
public:
	raster_layer(void);
	~raster_layer(void);

	bool load(const QString & url);

protected:
	virtual void gl_draw();
	QRect calc_tif_rect(const QRectF & geo_rect);
	QImage get_image(const QRect & rect, int width, int height);

private:
	QString m_url;
	GDALDataset * m_data;
};

}
