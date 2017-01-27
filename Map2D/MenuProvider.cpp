#include "MenuProvider.h"
#include "QMenu"
#include "qgisgui.h"
#include "QgsProviderRegistry.h"
#include "QPluginLoader"
#include "LayerInterface.h"
#include "QCoreApplication"
#include "QgsOpenVectorLayerDialog.h"
#include "component/GlobalInstance.h"
#include "qgsdataitem.h"

MenuProvider::MenuProvider(void)
{
}


MenuProvider::~MenuProvider(void)
{
}

QMenu* MenuProvider::createContextMenu()
{
	QMenu *menu = new QMenu;
	connect(menu->addAction(QgsLayerItem::iconRaster(), QObject::tr("添加栅格图层")), SIGNAL(triggered()), this, SLOT(slotAddRasterLayer()));
	connect(menu->addAction(QgsLayerItem::iconTable(), QObject::tr("添加矢量图层")), SIGNAL(triggered()), this, SLOT(slotAddVectorLayer()));
	connect(menu->addAction(QgsLayerItem::iconRaster(), QObject::tr("添加高程图层")), SIGNAL(triggered()), this, SLOT(slotAddDemLayer()));

	return menu;
}

void MenuProvider::slotAddRasterLayer()
{
	QString fileFilters;

	QStringList selectedFiles;
	QString e;//only for parameter correctness
	QString title = tr( "打开GDAL支持的栅格图层" );
	QgisGui::openFilesRememberingFilter( "lastRasterFileFilter", QgsProviderRegistry::instance()->fileRasterFilters(), selectedFiles, e,
		title );

	if ( selectedFiles.isEmpty() )
	{
		// no files were selected, so just bail
		return;
	}

	LayerInterface *layerFace = qobject_cast<LayerInterface *>(QPluginLoader(qApp->applicationDirPath() + "\\components\\Map2D.dll").instance());
	LayerInterface *layerFace3D = qobject_cast<LayerInterface *>(QPluginLoader(qApp->applicationDirPath() + "\\components\\Map3D.dll").instance());
	for(auto iter = selectedFiles.begin(); iter != selectedFiles.end(); ++iter){
		QFileInfo fi(*iter);
		if(layerFace) layerFace->addRasterLayer(fi.baseName(), *iter);
		if(layerFace3D) layerFace3D->addRasterLayer(fi.baseName(), *iter);
	}
}

void MenuProvider::slotAddVectorLayer()
{
	QStringList selectedFiles;
	QgisGui::openFilesRememberingFilter( "lastVectorFileFilter", QgsProviderRegistry::instance()->fileVectorFilters()
				, selectedFiles, tr("System"), tr("打开OGR支持的图层"));
	if ( !selectedFiles.isEmpty() )
	{
		LayerInterface *layerFace = qobject_cast<LayerInterface *>(QPluginLoader(qApp->applicationDirPath() + "\\components\\Map2D.dll").instance());
		LayerInterface *layerFace3D = qobject_cast<LayerInterface *>(QPluginLoader(qApp->applicationDirPath() + "\\components\\Map3D.dll").instance());
		for(auto iter = selectedFiles.begin(); iter != selectedFiles.end(); ++iter){
			QFileInfo fi(*iter);
			if(layerFace) layerFace->addVectorLayer(fi.baseName(), *iter);
			if(layerFace3D) layerFace3D->addVectorLayer(fi.baseName(), *iter);
		}
	}
}

void MenuProvider::slotAddDemLayer()
{
	QStringList selectedFiles;
	QString e;//only for parameter correctness
	QString title = tr( "打开GDAL支持的栅格图层" );
	QgisGui::openFilesRememberingFilter( "lastRasterFileFilter", QgsProviderRegistry::instance()->fileRasterFilters(), selectedFiles, e,
		title );

	if ( selectedFiles.isEmpty() )
	{
		// no files were selected, so just bail
		return;
	}

	LayerInterface *layerFace = qobject_cast<LayerInterface *>(QPluginLoader(qApp->applicationDirPath() + "\\components\\Map2D.dll").instance());
	LayerInterface *layerFace3D = qobject_cast<LayerInterface *>(QPluginLoader(qApp->applicationDirPath() + "\\components\\Map3D.dll").instance());
	for(auto iter = selectedFiles.begin(); iter != selectedFiles.end(); ++iter){
		QFileInfo fi(*iter);
		if(layerFace) layerFace->addElevationLayer(fi.baseName(), *iter);
		if(layerFace3D) layerFace3D->addElevationLayer(fi.baseName(), *iter);
	}
}
