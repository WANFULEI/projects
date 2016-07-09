///////////////////////////////////////////////////////////
//  File.cpp
//  Implementation of the Class File
//  Created on:      19-ÎåÔÂ-2016 21:13:40
//  Original author: wq
///////////////////////////////////////////////////////////

#include "File.h"

namespace framecore
{

file::file(){

}



file::~file(){

}





share_file_type file::get_file_type(){

	return  m_file_type;
}


const QString file::get_path(){

	return  m_path;
}


const bool file::is_dirty(){

	return m_is_dirty;
}


const bool file::is_load(){

	return m_is_load;
}


bool file::load_file(const QString & path){

	return false;
}


bool file::save_file(const QString & path){

	return false;
}


void file::set_file_type(const file_type & type){
	m_file_type = new file_type(type);
}

void file::set_file_type(const share_file_type & type)
{
	m_file_type = type;
}

void file::set_is_dirty(bool dirty){
	m_is_dirty = dirty;
}


void file::set_is_load(bool load){
	m_is_load = load;
}


void file::set_path(const QString & path){
	m_path = path;
}

}