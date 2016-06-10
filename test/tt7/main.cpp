
#include <QtCore/QCoreApplication>
#include "gdal_priv.h"
#include "cpl_conv.h" // for CPLMalloc()
#include <QtGui/QtGui>
#include <windows.h>
#include "simple_map.h"
#include "raster_layer.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

//	GDALDataset  *poDataset;

// 	GDALAllRegister();
// 
 	char * pszFilename = "F:\\earth-map\\NE2_HR_LC_SR_W_DR (2)\\NE2_HR_LC_SR_W_DR\\NE2_HR_LC_SR_W_DR.tif";
// 
// 	poDataset = (GDALDataset *) GDALOpen( pszFilename, GA_ReadOnly );
// 	if( poDataset == NULL )
// 	{
// 		
// 		;
// 	}
// 
// 	double        adfGeoTransform[6];
// 
// 	printf( "Driver: %s/%s\n",
// 		poDataset->GetDriver()->GetDescription(), 
// 		poDataset->GetDriver()->GetMetadataItem( GDAL_DMD_LONGNAME ) );
// 
// 	printf( "Size is %dx%dx%d\n", 
// 		poDataset->GetRasterXSize(), poDataset->GetRasterYSize(),
// 		poDataset->GetRasterCount() );
// 
// 	if( poDataset->GetProjectionRef()  != NULL )
// 		printf( "Projection is `%s'\n", poDataset->GetProjectionRef() );
// 
// 	if( poDataset->GetGeoTransform( adfGeoTransform ) == CE_None )
// 	{
// 		printf( "Origin = (%.6f,%.6f)\n",
// 			adfGeoTransform[0], adfGeoTransform[3] );
// 
// 		printf( "Pixel Size = (%.6f,%.6f)\n",
// 			adfGeoTransform[1], adfGeoTransform[5] );
// 	}
// 	GDALRasterBand  *poBand;
// 	int             nBlockXSize, nBlockYSize;
// 	int             bGotMin, bGotMax;
// 	double          adfMinMax[2];
// 
// 	poBand = poDataset->GetRasterBand( 1 );
// 	poBand->GetBlockSize( &nBlockXSize, &nBlockYSize );
// 	printf( "Block=%dx%d Type=%s, ColorInterp=%s\n",
// 		nBlockXSize, nBlockYSize,
// 		GDALGetDataTypeName(poBand->GetRasterDataType()),
// 		GDALGetColorInterpretationName(
// 		poBand->GetColorInterpretation()) );
// 
// 	adfMinMax[0] = poBand->GetMinimum( &bGotMin );
// 	adfMinMax[1] = poBand->GetMaximum( &bGotMax );
// 	if( ! (bGotMin && bGotMax) )
// 		GDALComputeRasterMinMax((GDALRasterBandH)poBand, TRUE, adfMinMax);
// 
// 	printf( "Min=%.3fd, Max=%.3f\n", adfMinMax[0], adfMinMax[1] );
// 
// 	if( poBand->GetOverviewCount() > 0 )
// 		printf( "Band has %d overviews.\n", poBand->GetOverviewCount() );
// 
// 	if( poBand->GetColorTable() != NULL )
// 		printf( "Band has a color table with %d entries.\n", 
// 		poBand->GetColorTable()->GetColorEntryCount() );
// 
// // 	QWidget w;
// // 	w.show();
// 
// 	byte * data = new byte[2048 * 2048];
// 	poBand->RasterIO( GF_Read, 0, 0, 2048, 2048, 
// 		data, 2048, 2048, GDT_Byte, 
// 		0, 0 );
// 
// 	poBand = poDataset->GetRasterBand( 2 );
// 	byte * data2 = new byte[2048 * 2048];
// 	poBand->RasterIO( GF_Read, 0, 0, 2048, 2048, 
// 		data2, 2048, 2048, GDT_Byte, 
// 		0, 0 );
// 
// 	poBand = poDataset->GetRasterBand( 3 );
// 	byte * data3 = new byte[2048 * 2048];
// 	poBand->RasterIO( GF_Read, 0, 0, 2048, 2048, 
// 		data3, 2048, 2048, GDT_Byte, 
// 		0, 0 );
// 
// 
// 	
// 
// //	QImage image((uchar *)data,256,256,QImage::Format_ARGB32);
// 
// 	QImage image(2048,2048,QImage::Format_RGB32);
// 	for (int i=0;i<2048;++i)
// 	{
// 		for (int j=0;j<2048;++j)
// 		{
// 			image.setPixel(i,j,RGB(data3[2048 * j + i],data2[2048 * j + i],data[2048 * j + i]));
// 		}
// 	}
// 
// 	image.save("f:\\1.png");

	simple_map map;
	map.set_window(-18,18,-9,9);
	raster_layer * layer = new raster_layer;
	layer->load(pszFilename);
	map.attach(layer);
	map.select_tool(layerwidget::Pan);
	map.show();

	return a.exec();
}
