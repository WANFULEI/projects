
#include <QtCore/QCoreApplication>
#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "cpl_conv.h" // for CPLMalloc()
#include <QtGui/QtGui>
#include <windows.h>
#include "simple_map.h"
#include "raster_layer.h"
#include "vector_layer.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QTextCodec *codec = QTextCodec::codecForName("System");  
	QTextCodec::setCodecForCStrings(codec);  
	QTextCodec::setCodecForLocale(codec);  
	QTextCodec::setCodecForTr(codec);  

//	GDALDataset  *poDataset;

// 	GDALAllRegister();
// 
 	char * pszFilename = "NE2_HR_LC_SR_W_DR.tif";
	QDir::setCurrent(a.applicationDirPath());
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

	GDALAllRegister();

// 	GDALDataset       *poDS;
// 
// 	poDS = (GDALDataset*) GDALOpenEx( "CHN_adm1.shp", GDAL_OF_VECTOR, NULL, NULL, NULL );
// 	if( poDS == NULL )
// 	{
// 		printf( "Open failed.\n" );
// 		exit( 1 );
// 	}
// 
// 	OGRLayer  *poLayer;
// 
// 	for(int i=0;i<poDS->GetLayerCount();++i)
// 	{
// 		poLayer = poDS->GetLayer(i);
// 		if (poLayer == 0)
// 		{
// 			continue;
// 		}
// 
// 
// 		OGRFeature *poFeature;
// 
// 		poLayer->ResetReading();
// 		while( (poFeature = poLayer->GetNextFeature()) != NULL )
// 		{
// 			OGRFeatureDefn *poFDefn = poLayer->GetLayerDefn();
// 			int iField;
// 
// 			for( iField = 0; iField < poFDefn->GetFieldCount(); iField++ )
// 			{
// 				OGRFieldDefn *poFieldDefn = poFDefn->GetFieldDefn( iField );
// 
// 				if( poFieldDefn->GetType() == OFTInteger )
// 					printf( "%d,", poFeature->GetFieldAsInteger( iField ) );
// 				else if( poFieldDefn->GetType() == OFTInteger64 )
// 					printf( CPL_FRMT_GIB ",", poFeature->GetFieldAsInteger64( iField ) );
// 				else if( poFieldDefn->GetType() == OFTReal )
// 					printf( "%.3f,", poFeature->GetFieldAsDouble(iField) );
// 				else if( poFieldDefn->GetType() == OFTString )
// 					printf( "%s,", poFeature->GetFieldAsString(iField) );
// 				else
// 					printf( "%s,", poFeature->GetFieldAsString(iField) );
// 			}
// 			printf("\n\n\n");
// 
// 			OGRGeometry *poGeometry;
// 
// 			poGeometry = poFeature->GetGeometryRef();
// 			if( poGeometry != NULL 
// 				&& wkbFlatten(poGeometry->getGeometryType()) == wkbPoint )
// 			{
// 				OGRPoint *poPoint = (OGRPoint *) poGeometry;
// 
// 				printf( "%.3f,%3.f\n", poPoint->getX(), poPoint->getY() );
// 			}
// 			else if( poGeometry != NULL 
// 				&& wkbFlatten(poGeometry->getGeometryType()) == wkbPolygon )
// 			{
// 				OGRPolygon *poPolygon = (OGRPolygon *) poGeometry;
// 
// 				//printf( "%.3f,%3.f\n", poPoint->getX(), poPoint->getY() );
// 
// 				OGRCurve * curve = poPolygon->getExteriorRingCurve();
// 				OGRPointIterator * iter = curve->getPointIterator();
// 				OGRPoint pt;
// // 				while (iter->getNextPoint(&pt))
// // 				{
// // 					printf( "%.3f,%3.f\n", pt.getX(), pt.getY() );
// // 				}
// 			}
// 			else if( poGeometry != NULL 
// 				&& wkbFlatten(poGeometry->getGeometryType()) == wkbLineString )
// 			{
// 				OGRPoint *poPoint = (OGRPoint *) poGeometry;
// 
// 				printf( "%.3f,%3.f\n", poPoint->getX(), poPoint->getY() );
// 			}
// 			else
// 			{
// 				printf( "no point geometry\n" );
// 			}
// 			OGRFeature::DestroyFeature( poFeature );
// 		}
// 
// 	}
// 	GDALClose( poDS );



	simple_map map;
	map.set_window(-18,18,-1,1);
	raster_layer * layer = new raster_layer;
	layer->load("data\\NE2_HR_LC_SR_W_DR.tif");
	map.attach(layer);
 	vector_layer * vlayer = new vector_layer;
 	vlayer->load("data\\ne_10m_admin_0_countries.shp");
 	map.attach(vlayer);

	map.select_tool(layerwidget::Pan);
	map.show();
	map.redraw();
	return a.exec();
}
