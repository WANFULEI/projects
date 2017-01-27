#pragma once

#include "QgsPluginLayer.h"
#include "GraphicItem.h"
#include <osg/Group>


class GraphicLayer : public QgsPluginLayer
{
public:
	GraphicLayer(QString name);
	~GraphicLayer(void);

	template<class T>
	T *createGraphic(){
		T *p = new T;
		p->setLayer(this);
		m_graphics << p;
		m_group->addChild(p->getFeatureNode());
		return p;
	}
	void addGraphic(GraphicItem *gra);
	void removeGraphic(GraphicItem *gra);
	void removeGraphic(QString ID);
	GraphicItem *getGraphic(QString ID);
	QList<GraphicItem *> getAll() const { return m_graphics; }

	osg::Group *getGroup() const { return m_group; }

protected:
	virtual bool draw(QgsRenderContext& rendererContext);

	virtual void setAbstract(const QString& abstract);

	virtual QString abstract() const;

	virtual bool readSymbology(const QDomNode& node, QString& errorMessage);

	virtual bool writeSymbology(QDomNode &node, QDomDocument& doc, QString& errorMessage) const;

private:
	QList<GraphicItem *> m_graphics;
	osg::Group *m_group;
};

