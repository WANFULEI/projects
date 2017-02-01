


#include "gdal_priv.h"
#include "gdal_alg.h"
#include "gdalwarper.h"

#include <cassert>

#ifdef _DEBUG
#pragma comment(lib, "gdal_i.lib")
#else
#pragma comment(lib, "gdal_i.lib")
#endif

#include <QDir>
#include <QFile>
#include <iostream>
using namespace std;

#define SIZEOFARRAY(array) sizeof(array)/sizeof(array[0])

static double dfMinX=0.0, dfMinY=0.0, dfMaxX=0.0, dfMaxY=0.0;
static double dfXRes=0.0, dfYRes=0.0;

static int bTargetAlignedPixels = FALSE;
static int nForcePixels=0, nForceLines=0, bQuiet = FALSE;
static int bEnableDstAlpha = FALSE, bEnableSrcAlpha = FALSE;
static int bVRT = FALSE;

char *pszDstFileName = 0;
// static char *apszSrcFiles[] = 
// {
// 	/*"../data/H50G038013DOM.tif",
// 	"../data/H50G038014DOM.tif",
// 	"../data/H50G038015DOM.tif",
// 	"../data/H50G039012DOM.tif",
// 	"../data/H50G039013DOM.tif",
// 	"../data/H50G039014DOM.tif",*/
// 	"E:\\LocalSpaceViewer\\LocaSpaceViewer\\download\\TaskIMG01131404\\Export14-09-13.tif",
// 	"E:\\LocalSpaceViewer\\LocaSpaceViewer\\download\\TaskIMG01131404\\Export14-09-139_0_2.tif"
// };

char **apszSrcFiles = 0;

GDALDataType eWorkingType = GDT_Byte;

static GDALResampleAlg eResampleAlg = GRA_NearestNeighbour;
static GDALDatasetH GDALWarpCreateOutput( char **papszSrcFiles, 
	int nFilesCount, 
	const char *pszFilename, 
	const char *pszFormat, 
	GDALDataType eDT );

static void CreateMask(char* const* paFilePath, const int nFilesCount);

int main(int argc, char* argv[])
{
	string path;
	do{
		cout << "please enter input dir." << endl;
		cin >> path;
	}while(!QDir(path.c_str()).exists());
	string outFilePath;
	do{
		cout << "please enter output file path." << endl;
		cin >> outFilePath;
	}while(QFile::exists(outFilePath.c_str()));
	
	QDir dir(path.c_str());
	QStringList files = dir.entryList(QStringList() << "*.tif", QDir::Files);
	apszSrcFiles = new char *[files.size()];
	for(int i=0; i<files.size(); ++i){
		QString s = (dir.absolutePath() + "\\" + files[i]).toStdString().c_str();
		apszSrcFiles[i] = new char[s.length() + 1];
		strcpy(apszSrcFiles[i], s.toStdString().c_str());
	}
	pszDstFileName = (char *)outFilePath.c_str();

	GDALAllRegister();

	GDALDatasetH hSrcDS = NULL;
	GDALDatasetH hDstDS = NULL;

	int nFilesCount = files.size();
	hDstDS = GDALWarpCreateOutput(apszSrcFiles, nFilesCount,
		pszDstFileName, "GTiff", GDT_Byte);

	char **papszSrcFiles = apszSrcFiles;
	CreateMask(papszSrcFiles, nFilesCount);


	for( int iSrc = 0; iSrc < nFilesCount; iSrc++ )
	{
		GDALDatasetH hSrcDS;

		hSrcDS = GDALOpen( apszSrcFiles[iSrc], GA_ReadOnly );
		GDALDataset *pSrcDS = (GDALDataset *)hSrcDS;
		assert(pSrcDS->GetRasterBand(1)->GetMaskBand() != NULL);

		if( hSrcDS == NULL )
			exit( 2 );

		if ( GDALGetRasterCount(hSrcDS) == 0 )
		{
			fprintf(stderr, "Input file %s has no raster bands.\n", apszSrcFiles[iSrc] );
			exit( 1 );
		}

		if( !bQuiet )
			printf( "Processing input file %s.\n", apszSrcFiles[iSrc] );

		if ( eResampleAlg != GRA_NearestNeighbour &&
			GDALGetRasterColorTable(GDALGetRasterBand(hSrcDS, 1)) != NULL)
		{
			if( !bQuiet )
				fprintf( stderr, "Warning: Input file %s has a color table, which will likely lead to "
				"bad results when using a resampling method other than "
				"nearest neighbour. Converting the dataset prior to 24/32 bit "
				"is advised.\n", apszSrcFiles[iSrc] );
		}

		if( GDALGetRasterColorInterpretation( 
			GDALGetRasterBand(hSrcDS,GDALGetRasterCount(hSrcDS)) ) 
			== GCI_AlphaBand 
			&& !bEnableSrcAlpha )
		{
			bEnableSrcAlpha = TRUE;
			if( !bQuiet )
				printf( "Using band %d of source image as alpha.\n", 
				GDALGetRasterCount(hSrcDS) );
		}
		void *hTransformArg = NULL;

		hTransformArg = GDALCreateGenImgProjTransformer2( hSrcDS, hDstDS, NULL );

		if( hTransformArg == NULL )
			exit( 1 );

		GDALTransformerFunc pfnTransformer = GDALGenImgProjTransform;

		GDALWarpOptions *psWO = GDALCreateWarpOptions();

		psWO->papszWarpOptions = NULL;
		psWO->eWorkingDataType = eWorkingType;
		psWO->eResampleAlg = eResampleAlg;

		psWO->hSrcDS = hSrcDS;
		psWO->hDstDS = hDstDS;

		psWO->pfnTransformer = pfnTransformer;
		psWO->pTransformerArg = hTransformArg;

		if( !bQuiet )
			psWO->pfnProgress = GDALTermProgress;


		if( bEnableSrcAlpha )
			psWO->nBandCount = GDALGetRasterCount(hSrcDS) - 1;
		else
			psWO->nBandCount = GDALGetRasterCount(hSrcDS);

		psWO->panSrcBands = (int *) CPLMalloc(psWO->nBandCount*sizeof(int));
		psWO->panDstBands = (int *) CPLMalloc(psWO->nBandCount*sizeof(int));

		for( int nBandIndex = 0; nBandIndex < psWO->nBandCount; nBandIndex++ )
		{
			psWO->panSrcBands[nBandIndex] = nBandIndex+1;
			psWO->panDstBands[nBandIndex] = nBandIndex+1;
		}

		if( bEnableSrcAlpha )
			psWO->nSrcAlphaBand = GDALGetRasterCount(hSrcDS);

		if( !bEnableDstAlpha 
			&& GDALGetRasterCount(hDstDS) == psWO->nBandCount+1 
			&& GDALGetRasterColorInterpretation( 
			GDALGetRasterBand(hDstDS,GDALGetRasterCount(hDstDS))) 
			== GCI_AlphaBand )
		{
			if( !bQuiet )
				printf( "Using band %d of destination image as alpha.\n", 
				GDALGetRasterCount(hDstDS) );

			bEnableDstAlpha = TRUE;
		}

		GDALWarpOperation oWO;

		if( oWO.Initialize( psWO ) == CE_None )
		{
			CPLErr eErr;
			eErr = oWO.ChunkAndWarpImage( 0, 0,
				GDALGetRasterXSize( hDstDS ),
				GDALGetRasterYSize( hDstDS ) );
		}

		GDALDestroyGenImgProjTransformer( hTransformArg );

		GDALDestroyWarpOptions( psWO );

		GDALClose( hSrcDS );
	}

	GDALClose( hDstDS );

// 	for(int i=0; nFilesCount; ++i){
// 		delete [] apszSrcFiles[i];
// 	}
// 	delete [] apszSrcFiles;
	return 0;
}


static GDALDatasetH 
	GDALWarpCreateOutput( char **papszSrcFiles, int nFilesCount,
	const char *pszFilename, const char *pszFormat, 
	GDALDataType eDT )


{
	GDALDriverH hDriver;
	GDALDatasetH hDstDS;
	void *hTransformArg;
	GDALColorTableH hCT = NULL;
	double dfWrkMinX=0, dfWrkMaxX=0, dfWrkMinY=0, dfWrkMaxY=0;
	double dfWrkResX=0, dfWrkResY=0;
	int nDstBandCount = 0;

	hDriver = GDALGetDriverByName( pszFormat );
	if( hDriver == NULL 
		|| GDALGetMetadataItem( hDriver, GDAL_DCAP_CREATE, NULL ) == NULL )
	{
		int	iDr;

		printf( "Output driver `%s' not recognised or does not support\n", 
			pszFormat );
		printf( "direct output file creation.  The following format drivers are configured\n"
			"and support direct output:\n" );

		for( iDr = 0; iDr < GDALGetDriverCount(); iDr++ )
		{
			GDALDriverH hDriver = GDALGetDriver(iDr);

			if( GDALGetMetadataItem( hDriver, GDAL_DCAP_CREATE, NULL) != NULL )
			{
				printf( "  %s: %s\n",
					GDALGetDriverShortName( hDriver  ),
					GDALGetDriverLongName( hDriver ) );
			}
		}
		printf( "\n" );
		exit( 1 );
	}

	string proj;
	int iSrc = 0;
	for( iSrc = 0; iSrc < nFilesCount; iSrc++ )
	{
		GDALDatasetH hSrcDS;

		hSrcDS = GDALOpen( papszSrcFiles[iSrc], GA_ReadOnly );
		if( hSrcDS == NULL )
			exit( 1 );

		if(iSrc == 0) proj = GDALGetProjectionRef(hSrcDS);

		if ( GDALGetRasterCount(hSrcDS) == 0 )
		{
			fprintf(stderr, "Input file %s has no raster bands.\n", papszSrcFiles[iSrc] );
			exit( 1 );
		}

		if( eDT == GDT_Unknown )
			eDT = GDALGetRasterDataType(GDALGetRasterBand(hSrcDS,1));

		if( iSrc == 0 )
		{
			nDstBandCount = GDALGetRasterCount(hSrcDS);
			hCT = GDALGetRasterColorTable( GDALGetRasterBand(hSrcDS,1) );
			if( hCT != NULL )
			{
				hCT = GDALCloneColorTable( hCT );
			}
		}

		hTransformArg = 
			GDALCreateGenImgProjTransformer2( hSrcDS, NULL, NULL);

		if( hTransformArg == NULL )
		{
			GDALClose( hSrcDS );
			return NULL;
		}

		double adfThisGeoTransform[6];
		double adfExtent[4];
		int    nThisPixels, nThisLines;

		if( GDALSuggestedWarpOutput2( hSrcDS, 
			GDALGenImgProjTransform, hTransformArg, 
			adfThisGeoTransform, 
			&nThisPixels, &nThisLines, 
			adfExtent, 0 ) != CE_None )
		{
			GDALClose( hSrcDS );
			return NULL;
		}

		if (CPLGetConfigOption( "CHECK_WITH_INVERT_PROJ", NULL ) == NULL)
		{
			double MinX = adfExtent[0];
			double MaxX = adfExtent[2];
			double MaxY = adfExtent[3];
			double MinY = adfExtent[1];
			int bSuccess = TRUE;

#define N_STEPS 20
			int i = 0;
			int j = 0;
			for(i=0; i <= N_STEPS && bSuccess; i++)
			{
				for(j=0; j <= N_STEPS && bSuccess; j++)
				{
					double dfRatioI = i * 1.0 / N_STEPS;
					double dfRatioJ = j * 1.0 / N_STEPS;
					double expected_x = (1 - dfRatioI) * MinX + dfRatioI * MaxX;
					double expected_y = (1 - dfRatioJ) * MinY + dfRatioJ * MaxY;
					double x = expected_x;
					double y = expected_y;
					double z = 0;
					if (!GDALGenImgProjTransform(hTransformArg, TRUE, 1, &x, &y, &z, &bSuccess) 
						|| !bSuccess)
						bSuccess = FALSE;

					if (!GDALGenImgProjTransform(hTransformArg, FALSE, 1, &x, &y, &z, &bSuccess) 
						|| !bSuccess)
						bSuccess = FALSE;
					if (fabs(x - expected_x) > (MaxX - MinX) / nThisPixels ||
						fabs(y - expected_y) > (MaxY - MinY) / nThisLines)
						bSuccess = FALSE;
				}
			}

			if (!bSuccess)
			{
				CPLSetConfigOption( "CHECK_WITH_INVERT_PROJ", "TRUE" );
				CPLDebug("WARP", "Recompute out extent with CHECK_WITH_INVERT_PROJ=TRUE");
				GDALDestroyGenImgProjTransformer(hTransformArg);
				hTransformArg = 
					GDALCreateGenImgProjTransformer2( hSrcDS, NULL, NULL );

				if( GDALSuggestedWarpOutput2( hSrcDS, 
					GDALGenImgProjTransform, hTransformArg, 
					adfThisGeoTransform, 
					&nThisPixels, &nThisLines, 
					adfExtent, 0 ) != CE_None )
				{
					GDALClose( hSrcDS );
					return NULL;
				}
			}
		}

		if( dfWrkMaxX == 0.0 && dfWrkMinX == 0.0 )
		{
			dfWrkMinX = adfExtent[0];
			dfWrkMaxX = adfExtent[2];
			dfWrkMaxY = adfExtent[3];
			dfWrkMinY = adfExtent[1];
			dfWrkResX = adfThisGeoTransform[1];
			dfWrkResY = ABS(adfThisGeoTransform[5]);
		}
		else
		{
			dfWrkMinX = MIN(dfWrkMinX,adfExtent[0]);
			dfWrkMaxX = MAX(dfWrkMaxX,adfExtent[2]);
			dfWrkMaxY = MAX(dfWrkMaxY,adfExtent[3]);
			dfWrkMinY = MIN(dfWrkMinY,adfExtent[1]);
			dfWrkResX = MIN(dfWrkResX,adfThisGeoTransform[1]);
			dfWrkResY = MIN(dfWrkResY,ABS(adfThisGeoTransform[5]));
		}

		GDALDestroyGenImgProjTransformer( hTransformArg );

		GDALClose( hSrcDS );
	}

	if( nDstBandCount == 0 )
	{
		CPLError( CE_Failure, CPLE_AppDefined,
			"No usable source images." );
		return NULL;
	}

	double adfDstGeoTransform[6];
	int nPixels, nLines;

	adfDstGeoTransform[0] = dfWrkMinX;
	adfDstGeoTransform[1] = dfWrkResX;
	adfDstGeoTransform[2] = 0.0;
	adfDstGeoTransform[3] = dfWrkMaxY;
	adfDstGeoTransform[4] = 0.0;
	adfDstGeoTransform[5] = -1 * dfWrkResY;

	nPixels = (int) ((dfWrkMaxX - dfWrkMinX) / dfWrkResX + 0.5);
	nLines = (int) ((dfWrkMaxY - dfWrkMinY) / dfWrkResY + 0.5);

	if( dfXRes != 0.0 && dfYRes != 0.0 )
	{
		if( dfMinX == 0.0 && dfMinY == 0.0 && dfMaxX == 0.0 && dfMaxY == 0.0 )
		{
			dfMinX = adfDstGeoTransform[0];
			dfMaxX = adfDstGeoTransform[0] + adfDstGeoTransform[1] * nPixels;
			dfMaxY = adfDstGeoTransform[3];
			dfMinY = adfDstGeoTransform[3] + adfDstGeoTransform[5] * nLines;
		}

		if ( bTargetAlignedPixels )
		{
			dfMinX = floor(dfMinX / dfXRes) * dfXRes;
			dfMaxX = ceil(dfMaxX / dfXRes) * dfXRes;
			dfMinY = floor(dfMinY / dfYRes) * dfYRes;
			dfMaxY = ceil(dfMaxY / dfYRes) * dfYRes;
		}

		nPixels = (int) ((dfMaxX - dfMinX + (dfXRes/2.0)) / dfXRes);
		nLines = (int) ((dfMaxY - dfMinY + (dfYRes/2.0)) / dfYRes);
		adfDstGeoTransform[0] = dfMinX;
		adfDstGeoTransform[3] = dfMaxY;
		adfDstGeoTransform[1] = dfXRes;
		adfDstGeoTransform[5] = -dfYRes;
	}
	else if( nForcePixels != 0 && nForceLines != 0 )
	{
		if( dfMinX == 0.0 && dfMinY == 0.0 && dfMaxX == 0.0 && dfMaxY == 0.0 )
		{
			dfMinX = dfWrkMinX;
			dfMaxX = dfWrkMaxX;
			dfMaxY = dfWrkMaxY;
			dfMinY = dfWrkMinY;
		}

		dfXRes = (dfMaxX - dfMinX) / nForcePixels;
		dfYRes = (dfMaxY - dfMinY) / nForceLines;

		adfDstGeoTransform[0] = dfMinX;
		adfDstGeoTransform[3] = dfMaxY;
		adfDstGeoTransform[1] = dfXRes;
		adfDstGeoTransform[5] = -dfYRes;

		nPixels = nForcePixels;
		nLines = nForceLines;
	}
	else if( nForcePixels != 0 )
	{
		if( dfMinX == 0.0 && dfMinY == 0.0 && dfMaxX == 0.0 && dfMaxY == 0.0 )
		{
			dfMinX = dfWrkMinX;
			dfMaxX = dfWrkMaxX;
			dfMaxY = dfWrkMaxY;
			dfMinY = dfWrkMinY;
		}

		dfXRes = (dfMaxX - dfMinX) / nForcePixels;
		dfYRes = dfXRes;

		adfDstGeoTransform[0] = dfMinX;
		adfDstGeoTransform[3] = dfMaxY;
		adfDstGeoTransform[1] = dfXRes;
		adfDstGeoTransform[5] = -dfYRes;

		nPixels = nForcePixels;
		nLines = (int) ((dfMaxY - dfMinY + (dfYRes/2.0)) / dfYRes);
	}
	else if( nForceLines != 0 )
	{
		if( dfMinX == 0.0 && dfMinY == 0.0 && dfMaxX == 0.0 && dfMaxY == 0.0 )
		{
			dfMinX = dfWrkMinX;
			dfMaxX = dfWrkMaxX;
			dfMaxY = dfWrkMaxY;
			dfMinY = dfWrkMinY;
		}

		dfYRes = (dfMaxY - dfMinY) / nForceLines;
		dfXRes = dfYRes;

		adfDstGeoTransform[0] = dfMinX;
		adfDstGeoTransform[3] = dfMaxY;
		adfDstGeoTransform[1] = dfXRes;
		adfDstGeoTransform[5] = -dfYRes;

		nPixels = (int) ((dfMaxX - dfMinX + (dfXRes/2.0)) / dfXRes);
		nLines = nForceLines;
	}
	else if( dfMinX != 0.0 || dfMinY != 0.0 || dfMaxX != 0.0 || dfMaxY != 0.0 )
	{
		dfXRes = adfDstGeoTransform[1];
		dfYRes = fabs(adfDstGeoTransform[5]);

		nPixels = (int) ((dfMaxX - dfMinX + (dfXRes/2.0)) / dfXRes);
		nLines = (int) ((dfMaxY - dfMinY + (dfYRes/2.0)) / dfYRes);

		dfXRes = (dfMaxX - dfMinX) / nPixels;
		dfYRes = (dfMaxY - dfMinY) / nLines;

		adfDstGeoTransform[0] = dfMinX;
		adfDstGeoTransform[3] = dfMaxY;
		adfDstGeoTransform[1] = dfXRes;
		adfDstGeoTransform[5] = -dfYRes;
	}

	if( bEnableSrcAlpha )
		nDstBandCount--;

	if( bEnableDstAlpha )
		nDstBandCount++;

	if( !bQuiet )
		printf( "Creating output file that is %dP x %dL.\n", nPixels, nLines );

	hDstDS = GDALCreate( hDriver, pszFilename, nPixels, nLines, 
		nDstBandCount, eDT, NULL );

	if( hDstDS == NULL )
	{
		return NULL;
	}

	GDALSetProjection(hDstDS, proj.c_str());

	GDALSetGeoTransform( hDstDS, adfDstGeoTransform );

	if( bEnableDstAlpha )
	{
		GDALSetRasterColorInterpretation( 
			GDALGetRasterBand( hDstDS, nDstBandCount ), 
			GCI_AlphaBand );
	}

	if( hCT != NULL )
	{
		GDALSetRasterColorTable( GDALGetRasterBand(hDstDS,1), hCT );
		GDALDestroyColorTable( hCT );
	}

	return hDstDS;
}

void CreateMask(char* const* paFilePath, const int nFilesCount)
{
	for ( int nSrcIndex = 0; nSrcIndex < nFilesCount; nSrcIndex++ )
	{
		GDALDatasetH hSrcDS;

		hSrcDS = GDALOpen( apszSrcFiles[nSrcIndex], GA_Update );

		GDALDataset *pSrcDS = (GDALDataset *)hSrcDS;
		pSrcDS->CreateMaskBand(GMF_ALL_VALID);

		for (int nBandIndex = 1; nBandIndex <= pSrcDS->GetRasterCount(); ++nBandIndex)
		{
			GDALRasterBand *pSrcBand = pSrcDS->GetRasterBand(nBandIndex);
			GDALRasterBand *pMaskBand = pSrcBand->GetMaskBand();

			double dfNoDataValue = pMaskBand->GetNoDataValue();
			dfNoDataValue = pSrcBand->GetNoDataValue();

			int nMaskXSize = pMaskBand->GetXSize();
			int nMaskYSize = pMaskBand->GetYSize();

			unsigned char *pMaskData = new unsigned char[nMaskXSize * nMaskYSize];  
			unsigned int *pMaskDataInt = (unsigned int *)pMaskData;

			pMaskBand->RasterIO(GF_Read,0, 0, nMaskXSize, nMaskYSize, 
				pMaskData, nMaskXSize, nMaskYSize, GDT_Byte, 0, 0);

			unsigned char *pRasterData = new unsigned char[pSrcBand->GetXSize() * 
				pSrcBand->GetYSize()];
			pSrcBand->RasterIO(GF_Read,0, 0, pSrcBand->GetXSize(), pSrcBand->GetYSize(), 
				pRasterData, pSrcBand->GetXSize(), pSrcBand->GetYSize(), GDT_Byte, 0, 0);

			for ( int nMaskPos = 0; nMaskPos < nMaskXSize * nMaskYSize; ++nMaskPos )
			{
				if (pRasterData[nMaskPos] == 0)
				{
					pMaskData[nMaskPos] = 0x00;
				}
				else
				{
					pMaskData[nMaskPos] = 0xFF;
				}
			}

			pMaskBand->RasterIO(GF_Write, 0, 0, pMaskBand->GetXSize(), 
				pMaskBand->GetYSize(), pMaskData, nMaskXSize, 
				nMaskYSize, GDT_Byte, 0, 0);

			delete []pRasterData;
			pRasterData = NULL;
			delete []pMaskData;			
			pMaskData = NULL;   
		}

		GDALClose(hSrcDS);
	}
}
