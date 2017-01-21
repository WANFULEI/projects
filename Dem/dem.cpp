#include "dem.h"
#include "gdal_priv.h"
#include "cpl_conv.h" // for CPLMalloc()

Dem::Dem(QString demFile)
	:m_demFilePath(demFile)
{
	GDALAllRegister();
	m_pDataset = 0;
	m_pBand = 0;
	for(int i=0; i<6; ++i) m_geoTransform[i] = 0;
}

Dem::~Dem()
{

}

short Dem::getHeight(double lon, double lat)
{
	if(m_pBand == 0) return -1;
 	double temp = m_geoTransform[1] * m_geoTransform[5] - m_geoTransform[2] * m_geoTransform[4];
	int col = (m_geoTransform[5] * (lon - m_geoTransform[0]) - m_geoTransform[2] * (lat - m_geoTransform[3])) / temp + 0.5;
	int row = (m_geoTransform[1] * (lat - m_geoTransform[3]) - m_geoTransform[4] * (lon - m_geoTransform[0])) / temp + 0.5;
	short value = 0;
	CPLErr err = m_pBand->RasterIO(GF_Read, col, row, 1, 1, &value, 1, 1, GDT_Int16, 0, 0);
	return value;
} 

QVector<QVector<short>> Dem::getHeight(double left, double right, double bottom, double top)
{
	QVector<QVector<short>> data;
	if(right - left < 0 || top - bottom < 0) return data;
	if(m_pBand == 0) return data;
	double temp = m_geoTransform[1] * m_geoTransform[5] - m_geoTransform[2] * m_geoTransform[4];
	int col = (m_geoTransform[5] * (left - m_geoTransform[0]) - m_geoTransform[2] * (top - m_geoTransform[3])) / temp + 0.5;
	int row = (m_geoTransform[1] * (top - m_geoTransform[3]) - m_geoTransform[4] * (left - m_geoTransform[0])) / temp + 0.5;
	int cols = (right - left) / m_geoTransform[1];
	int rows = (bottom - top) / m_geoTransform[5];
	short *pData = new short[rows * cols];
	m_pBand->RasterIO(GF_Read, col, row, cols, rows, pData, cols, rows, GDT_Int16, 0, 0);
	data.resize(rows);
	for(int i=0; i< rows; ++i){
		data[i].resize(cols);
		for(int j=0; j<cols; ++j){
			data[i][j] = pData[i * cols + j];
		}
	}
	return data;
}

bool Dem::open()
{
	m_pDataset = (GDALDataset *)GDALOpen(m_demFilePath.toStdString().c_str(), GA_ReadOnly);
	if(m_pDataset == 0) return false;
	m_pBand = m_pDataset->GetRasterBand(1);
	if(m_pBand == 0) return false;

	double        adfGeoTransform[6];

	printf( "Driver: %s/%s\n",
		m_pDataset->GetDriver()->GetDescription(),
		m_pDataset->GetDriver()->GetMetadataItem( GDAL_DMD_LONGNAME ) );

	printf( "Size is %dx%dx%d\n",
		m_pDataset->GetRasterXSize(), m_pDataset->GetRasterYSize(),
		m_pDataset->GetRasterCount() );

	if( m_pDataset->GetProjectionRef()  != NULL )
		printf( "Projection is `%s'\n", m_pDataset->GetProjectionRef() );

	if( m_pDataset->GetGeoTransform( adfGeoTransform ) == CE_None )
	{
		printf( "Origin = (%.6f,%.6f)\n",
			adfGeoTransform[0], adfGeoTransform[3] );

		printf( "Pixel Size = (%.6f,%.6f)\n",
			adfGeoTransform[1], adfGeoTransform[5] );
	}
	printf(GDALVersionInfo(""));

	return m_pDataset->GetGeoTransform(m_geoTransform) == CE_None;
}


