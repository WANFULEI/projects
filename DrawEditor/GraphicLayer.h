#pragma once

#include "QgsPluginLayer.h"

class GraphicItem;
class GraphicLayer : public QgsPluginLayer
{
public:
	GraphicLayer(QString name);
	~GraphicLayer(void);

	template<class T>
	T *createGraphic(){
		T *p = new T;
		m_graphics << p;
		return p;
	}
	void addGraphic(GraphicItem *gra);
	void removeGraphic(GraphicItem *gra);
	void removeGraphic(QString ID);
	GraphicItem *getGraphic(QString ID);
	QList<GraphicItem *> getAll() const { return m_graphics; }

protected:
	virtual bool draw(QgsRenderContext& rendererContext);

	virtual void setAbstract(const QString& abstract);

	virtual QString abstract() const;

	virtual bool readSymbology(const QDomNode& node, QString& errorMessage);

	virtual bool writeSymbology(QDomNode &node, QDomDocument& doc, QString& errorMessage) const;

private:
	QList<GraphicItem *> m_graphics;
};

