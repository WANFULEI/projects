#pragma once
#include <QString>
#include <tinyxml.h>

bool toBool(QString text){
	return (text.toLower() == "false" || text.toLower() == "no" || text == "0") ? false : true;
}

bool getAttribute(TiXmlElement *xmlNode, QString name, bool defaultValue){
	if(xmlNode == 0 || name.isEmpty()) return defaultValue;
	xmlNode->QueryBoolAttribute(name.toStdString().c_str(), &defaultValue);
	return defaultValue;
}

QString getAttribute(TiXmlElement *xmlNode, QString name, QString defaultValue = ""){
	if(xmlNode == 0 || name.isEmpty()) return defaultValue;
	const char *value = xmlNode->Attribute(name.toStdString().c_str());
	return (value == QString("")) ? defaultValue : value;
}

int getAttribute(TiXmlElement *xmlNode, QString name, int defaultValue){
	if(xmlNode == 0 || name.isEmpty()) return defaultValue;
	xmlNode->QueryIntAttribute(name.toStdString().c_str(), &defaultValue);
	return defaultValue;
}

double getAttribute(TiXmlElement *xmlNode, QString name, double defaultValue){
	if(xmlNode == 0 || name.isEmpty()) return defaultValue;
	xmlNode->QueryDoubleAttribute(name.toStdString().c_str(), &defaultValue);
	return defaultValue;
}