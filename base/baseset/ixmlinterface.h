#pragma once

#include "../base/tinyxml/tinyxml.h"

class IXmlInterface
{
	public:
		virtual TiXmlElement saveToXml() = 0;
		virtual bool readFromXml(TiXmlElement *xmlNode) = 0;	
		
}