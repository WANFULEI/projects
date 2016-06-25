#include "gl/glew.h"
#include "vector_layer.h"
#include "ogrsf_frmts.h"
#include <QTime>
#include <QDebug>
#include "..\..\template\time_elapsed.h"
#include "..\..\base\math\math2.h"
#include "..\..\control\layer_widget\layer_widget.h"
#include "log4cpp/Category.hh"



namespace map_wgt{

vector_layer::vector_layer(void)
{
	m_border_color = Qt::darkGray;
	m_fill_brush = Qt::transparent;
	m_border_width = 1;
	m_border_style = Qt::SolidLine;
	m_border_cap = Qt::SquareCap;
	m_border_join = Qt::BevelJoin;
}


vector_layer::~vector_layer(void)
{
	free_features();
}

bool vector_layer::load(const QString & url)
{
	GDALDataset * poDS = (GDALDataset *) GDALOpenEx( url.toStdString().c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL );
	if( poDS == NULL )
	{
		return false;
	}

	free_features();

	OGRLayer * poLayer = 0;


	for(int i=0;i<poDS->GetLayerCount();++i)
	{
		poLayer = poDS->GetLayer(i);
		if (poLayer == 0)
		{
			continue;
		}

		OGRFeature * poFeature = 0;
		poLayer->ResetReading();
		OGRFieldDefn def("buffer_id", OGRFieldType::OFTInteger);
		poLayer->GetLayerDefn()->AddFieldDefn(&def);
		OGRFieldDefn def1("vertex_count", OGRFieldType::OFTInteger);
		poLayer->GetLayerDefn()->AddFieldDefn(&def1);
		while( (poFeature = poLayer->GetNextFeature()) != 0 )
		{
			m_features << poFeature;
			

		}

	}
	GDALClose( poDS );
	m_url = url;

	update_envelope();


	return true;
}

void vector_layer::free_features()
{
	for (int i=0;i<m_features.size();++i)
	{
		OGRFeature::DestroyFeature( m_features[i] );
	}
}

void vector_layer::gl_draw()
{
	time_elapsed timer;

	QRectF viewport;
	viewport.setLeft(widget->x_min());
	viewport.setRight(widget->x_max());
	viewport.setTop(widget->y_min());
	viewport.setBottom(widget->y_max());

	if (!get_envelope().intersects(viewport))
	{
		return;
	}
	
// 	QPainter & pa = widget->get_painter();
// 	pa.save();
// 	pa.setPen(QPen(m_border_color,m_border_width,m_border_style,m_border_cap,m_border_join));
// 	pa.setBrush(m_fill_brush);

	int num = 0;
	for (int i=0;i<m_features.size();++i)
	{
		OGRGeometry * geometry = m_features[i]->GetGeometryRef();
		if (geometry == 0)
		{
			continue;
		}

		if (geometry->getGeometryType() == wkbPoint)
		{

		}
		else if ( geometry->getGeometryType() == wkbLineString )
		{

		}
		else if ( geometry->getGeometryType() == wkbPolygon )
		{
			if (!is_in_viewport(geometry, viewport))
			{
				continue;
			}
			//draw_polygon(geometry);

			OGRGeometry *geometry = m_features[i]->GetGeometryRef();
			draw_polygon2(geometry);


			++num;
		}
		else if (geometry->getGeometryType() == wkbMultiPolygon)
		{
			OGRMultiPolygon * polygon = (OGRMultiPolygon *)geometry;
			for (int j=0;j<polygon->getNumGeometries();++j)
			{
				auto geometry = polygon->getGeometryRef(j);
				if (geometry == 0)
				{
					continue;
				}
				if (geometry->getGeometryType() == wkbPolygon)
				{
					if (!is_in_viewport(geometry, viewport))
					{
						continue;
					}
					draw_polygon2((OGRPolygon *)geometry);
					++num;
				}
			}
		}
		else
		{
			auto type = geometry->getGeometryType();
			int n = 0;
		}
	}
	
	//pa.restore();
	qDebug() << "vector draw:" << timer.stop() / 1000 << "num:" << num;
	log4cpp::logger().info("vector draw:");
}

void vector_layer::update_envelope()
{
	if (m_features.size())
	{
		OGREnvelope envelope;
		m_envelope = QRectF();
		for (int i=0;i<m_features.size();++i)
		{
			OGRGeometry * geometry = m_features[i]->GetGeometryRef();
			if (geometry == 0)
			{
				continue;
			}
			geometry->getEnvelope(&envelope);
			QRectF rc(envelope.MinX,envelope.MinY,envelope.MaxX-envelope.MinX,envelope.MaxY-envelope.MinY);
			if (m_envelope.isEmpty())
			{
				m_envelope = rc;
			}
			else
			{
				m_envelope = m_envelope.united(rc);
			}
		}
	}
}

void vector_layer::draw_polygon(OGRGeometry * geometry)
{
	time_elapsed timer;
// 	geometry = geometry->Simplify(0.5);
// 	qDebug() << "Simplify:" << timer.stop();
	if (geometry == 0)
	{
		return;
	}
	if (geometry->getGeometryType() != wkbPolygon)
	{
		return;
	}
	

	OGRPolygon * polygon = (OGRPolygon *)geometry;
	OGRLinearRing * ring = polygon->getExteriorRing();
	OGRRawPoint * buffer = new OGRRawPoint[ring->getNumPoints()];

	ring->getPoints(buffer);
	timer.start();
// 	for (int j=0;j<ring->getNumPoints();++j)
// 	{
// 		buffer[j].x = widget->x_pixel(buffer[j].x);
// 		buffer[j].y = widget->y_pixel(buffer[j].y);
// 	}
	//			qDebug() << "getPoints:" << timer.stop();


	// 			auto tmp = pts.toList();
	// 			timer.start();
	// 			math::simplier_polyline(tmp,1);
	// 			qDebug() << "simplier_polyline:" << timer.stop();
	// 			pts = tmp.toVector();

	timer.start();
	//pa.drawPolygon((QPointF *)buffer,ring->getNumPoints());

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
// 	glEnable(GL_CULL_FACE);
// 	glCullFace(GL_BACK);
// 	glBegin(GL_POLYGON);
// 		for (int i=0; i<ring->getNumPoints(); ++i)
// 		{
// 			glVertex2f(buffer[i].x, buffer[i].y);
// 		}
// 	glEnd();

//	glEnableClientState(GL_VERTEX_ARRAY);
//	glVertexPointer(2, GL_DOUBLE, 0, buffer);
// 	glBegin(GL_POLYGON);
// 	 	for (int i=0; i<ring->getNumPoints(); ++i)
// 	 	{
// 	 		glArrayElement(i);
// 	 	}
// 	glEnd();
// 	int n = ring->getNumPoints();
// 	int *p = new int[n];
// 	for (int i=0; i<n; ++i)
// 	{
// 		p[i] = i;
// 	}
// 	glDrawElements(GL_POLYGON, n, GL_UNSIGNED_INT, p);
// 	delete [] p;

//	glDrawArrays(GL_POLYGON, 0, ring->getNumPoints());


	delete [] buffer;
	//qDebug() << "drawPolygon:" << timer.stop();
}

bool vector_layer::is_in_viewport(OGRGeometry * geometry, QRectF &viewport)
{
	OGREnvelope envelope;
	//		baseset::time_elapsed timer;
	geometry->getEnvelope(&envelope);
	//		qDebug() << "getEnvelope:" << timer.stop();

	QRectF rc(envelope.MinX,envelope.MinY,envelope.MaxX-envelope.MinX,envelope.MaxY-envelope.MinY);
	return viewport.intersects(rc);
}

void vector_layer::draw_polygon2(OGRGeometry * geometry)
{
	if (geometry == 0)
	{
		return;
	}
	auto iter = m_tmp.find(geometry);
	if (iter == m_tmp.end())
	{
		if(geometry->getGeometryType() == wkbPolygon){
			OGRPolygon *polygon = (OGRPolygon *)geometry;
			OGRLinearRing *ring = polygon->getExteriorRing();
			OGRRawPoint *buffer = new OGRRawPoint[ring->getNumPoints()];
			ring->getPoints(buffer);
			GLuint id;
			glGenBuffers(1, &id);

			glBindBuffer(GL_ARRAY_BUFFER, id);
			glBufferData(GL_ARRAY_BUFFER, ring->getNumPoints()*sizeof(OGRRawPoint), buffer, GL_STATIC_DRAW);
			delete [] buffer;
			m_tmp[geometry] = QPair<int, int>(id, ring->getNumPoints());
		}
	}

	iter = m_tmp.find(geometry);

	glBindBuffer(GL_ARRAY_BUFFER, iter.value().first);
	glVertexPointer(2, GL_DOUBLE, 0, 0);
	glDrawArrays(GL_POLYGON, 0, iter.value().second);
}

}