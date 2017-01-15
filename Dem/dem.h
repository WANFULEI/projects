#ifndef DEM_H
#define DEM_H

#include "dem_global.h"
#include <QString>
#include <QVector>

class GDALDataset;
class GDALRasterBand;
class DEM_EXPORT Dem
{
public:
	Dem(QString demFile);
	~Dem();

	void setDemFilePath(QString demFile) { m_demFilePath = demFile; }
	bool open();

	short getHeight(double lon, double lat);
	QVector<QVector<short>> getHeight(double left, double right, double bottom, double top);

private:
	QString m_demFilePath;
	GDALDataset *m_pDataset;
	GDALRasterBand *m_pBand;
	double m_geoTransform[6];

};

#endif // DEM_H
