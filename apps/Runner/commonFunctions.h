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

bool getElementText(TiXmlElement *xmlNode, QString nodeName, QString &value = QString()){
	if(xmlNode == 0) return false;
	TiXmlElement *node = xmlNode->FirstChildElement(nodeName.toStdString().c_str());
	if(node == 0) return false;
	QString text = node->GetText();
	if(text.isEmpty()) return false;
	value = text;
	return true;
}

bool getElementInt(TiXmlElement *xmlNode, QString nodeName, int &value){
	if(xmlNode == 0) return false;
	TiXmlElement *node = xmlNode->FirstChildElement(nodeName.toStdString().c_str());
	if(node == 0) return false;
	QString text = node->GetText();
	if(text.isEmpty()) return false;
	bool ok;
	int n = text.toInt(&ok);
	if(!ok) return false;
	value = n;
	return true;
}

bool getElementDouble(TiXmlElement *xmlNode, QString nodeName, double &value){
	if(xmlNode == 0) return false;
	TiXmlElement *node = xmlNode->FirstChildElement(nodeName.toStdString().c_str());
	if(node == 0) return false;
	QString text = node->GetText();
	if(text.isEmpty()) return false;
	bool ok;
	double n = text.toDouble(&ok);
	if(!ok) return false;
	value = n;
	return true;
}

bool getElementBool(TiXmlElement *xmlNode, QString nodeName, bool &value){
	if(xmlNode == 0) return false;
	TiXmlElement *node = xmlNode->FirstChildElement(nodeName.toStdString().c_str());
	if(node == 0) return false;
	QString text = node->GetText();
	if(text.isEmpty()) return false;
	text = text.toLower();
	if(text == "1" || text == "yes" || text == "true"){
		value = true; return true; }
	else if(text == "0" || text == "no" || text == "false"){
		value = false; return true; }
	else return false;
}