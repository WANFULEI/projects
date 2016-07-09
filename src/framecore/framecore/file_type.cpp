///////////////////////////////////////////////////////////
//  FileType.cpp
//  Implementation of the Class FileType
//  Created on:      19-ÎåÔÂ-2016 20:42:34
//  Original author: wq
///////////////////////////////////////////////////////////

#include "file_type.h"

namespace framecore{

file_type::file_type(){

}



file_type::~file_type(){

}





QString file_type::get_ext() const{

	return  m_ext;
}


void file_type::set_ext(const QString & ext){
	m_ext = ext;
	if (!m_ext.isEmpty())
	{
		if (m_ext.left(1) != ".")
		{
			m_ext = "." + m_ext;
		}
	}
}


QIcon file_type::get_icon() const{

	return  m_icon;
}


void file_type::set_icon(const QIcon & icon){
	m_icon = icon;
}

bool file_type::is_empty() const
{
	return m_ext.isEmpty();
}

}