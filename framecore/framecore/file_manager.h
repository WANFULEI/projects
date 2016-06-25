///////////////////////////////////////////////////////////
//  FileManager.h
//  Implementation of the Class FileManager
//  Created on:      19-ÎåÔÂ-2016 21:13:44
//  Original author: wq
///////////////////////////////////////////////////////////

#if !defined(EA_F0FD6FA5_B2DA_4d6a_9A28_D099E53B93DB__INCLUDED_)
#define EA_F0FD6FA5_B2DA_4d6a_9A28_D099E53B93DB__INCLUDED_

#include <QList>
#include "../../template/manager.h"
#include "../../template/singleton.h"
#include "file_type.h"
#include "File.h"
#include "framecore_global.h"

namespace framecore{

class FRAMECORE_EXPORT file_manager : public instance<file_manager>
{

public:
	file_manager();
	virtual ~file_manager();

	bool attach_file(const_share_file_ref file);

	template<typename T>
	share_file create_file(const file_type & type){
		if (type.is_empty())
		{
			return 0;
		}
		share_file_type type2 = get_file_type(type);
		if (type2 == 0)
		{
			register_file_type(type);
			type2 = get_file_type(type);
			if (type2 == 0)
			{
				return 0;
			}
		}
		share_file file = new T;
		file->set_file_type(type2);
		return file;
	}
	bool delete_file(const_share_file_ref file);
	bool detach_file(const_share_file_ref file);
	bool is_exist_file(const_share_file_ref file);

	bool register_file_type(const file_type & type);
	bool unregister_file_type(const file_type & type);
	bool is_exist_file_type(const file_type & type);
	share_file_type get_file_type(const file_type & type);
	
private:
	share_list_vector_manager<file> m_files;
	share_list_vector_manager<file_type> m_register_file_types;

};

}
#endif // !defined(EA_F0FD6FA5_B2DA_4d6a_9A28_D099E53B93DB__INCLUDED_)
