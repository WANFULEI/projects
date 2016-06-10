///////////////////////////////////////////////////////////
//  FileManager.cpp
//  Implementation of the Class FileManager
//  Created on:      19-ÎåÔÂ-2016 21:13:44
//  Original author: wq
///////////////////////////////////////////////////////////

#include "file_manager.h"

namespace framecore{

file_manager::file_manager(){

}



file_manager::~file_manager(){

}





bool file_manager::attach_file(const_share_file_ref file){
	if (is_exist_file(file))
	{
		return false;
	}
	m_files << file;
	return true;
}


// share_file file_manager::create_file(const file_type & type){
// 
// 	if (!is_exist_file_type(type))
// 	{
// 		return 0;
// 	}
// 	return 0;
// }


bool file_manager::delete_file(const_share_file_ref file){

	for (int i=0;i<m_files.size();++i)
	{
		if (m_files[i] == file)
		{
			m_files.erase(m_files.begin() + i);
			return true;
		}
	}
	return false;
}


bool file_manager::detach_file(const_share_file_ref file){
	for (int i=0;i<m_files.size();++i)
	{
		if (m_files[i] == file)
		{
			m_files.erase(m_files.begin() + i);
			return true;
		}
	}
	return false;
}


bool file_manager::register_file_type(const file_type & type){

	if (is_exist_file_type(type))
	{
		return false;
	}

	m_register_file_types.attach_entry(new file_type(type));

	return true;
}


bool file_manager::unregister_file_type(const file_type & type){
	for (int i=0;i<m_register_file_types.size();++i)
	{
		if (*m_register_file_types[i] == type)
		{
			m_register_file_types.erase(m_register_file_types.begin() + i);
			return true;
		}
	}
	return false;
}

bool file_manager::is_exist_file_type(const file_type & type)
{
	for (int i=0;i<m_register_file_types.size();++i)
	{
		if (*m_register_file_types[i] == type)
		{
			return true;
		}
	}
	return false;
}

framecore::share_file_type file_manager::get_file_type(const file_type & type)
{
	for (int i=0;i<m_register_file_types.size();++i)
	{
		if (*m_register_file_types[i] == type)
		{
			return m_register_file_types[i];
		}
	}
	return 0;
}

bool file_manager::is_exist_file(const_share_file_ref file)
{
	for(int i=0;i<m_files.size();++i)
	{
		if (m_files[i] == file)
		{
			return true;
		}
	}
	return false;
}

}