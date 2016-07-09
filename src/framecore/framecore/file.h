///////////////////////////////////////////////////////////
//  File.h
//  Implementation of the Class File
//  Created on:      19-ÎåÔÂ-2016 21:13:40
//  Original author: wq
///////////////////////////////////////////////////////////

#if !defined(EA_57CEB381_7E7F_4407_9BF0_88A003D7AF89__INCLUDED_)
#define EA_57CEB381_7E7F_4407_9BF0_88A003D7AF89__INCLUDED_

#include <QString>
#include "baseset/share_obj.h"
#include "file_type.h"
#include "framecore_global.h"

namespace framecore
{
class file_type;
class FRAMECORE_EXPORT file : public share_obj
{

public:
	file();
	virtual ~file();

	share_file_type get_file_type();
	const QString get_path();
	const bool is_dirty();
	const bool is_load();
	virtual bool load_file(const QString & path);
	virtual bool save_file(const QString & path);
	void set_file_type(const file_type & type);
	void set_file_type(const share_file_type & type);
	void set_is_dirty(bool dirty);
	void set_is_load(bool load);
	void set_path(const QString & path);

private:
	share_file_type m_file_type;
	bool m_is_dirty;
	bool m_is_load;
	QString m_path;

};

typedef share_ptr<file> share_file;
typedef const share_file & const_share_file_ref;

}
#endif // !defined(EA_57CEB381_7E7F_4407_9BF0_88A003D7AF89__INCLUDED_)
