///////////////////////////////////////////////////////////
//  FileType.h
//  Implementation of the Class FileType
//  Created on:      19-ÎåÔÂ-2016 20:42:34
//  Original author: wq
///////////////////////////////////////////////////////////

#if !defined(EA_85608C9D_C7A4_4ee8_AD57_C1DDC51C1D9F__INCLUDED_)
#define EA_85608C9D_C7A4_4ee8_AD57_C1DDC51C1D9F__INCLUDED_

#include <QString>
#include <QIcon>
#include "../../base/baseset2/baseset2.h"
#include "framecore_global.h"

namespace framecore{

class FRAMECORE_EXPORT file_type : public baseset::share_obj
{

public:
	file_type();
	virtual ~file_type();

	QString get_ext() const;
	void set_ext(const QString & ext);
	QIcon get_icon() const;
	void set_icon(const QIcon & icon);
	bool is_empty() const;

private:
	QString m_ext;
	QIcon m_icon;

};

typedef baseset::share_ptr<file_type> share_file_type;
typedef const share_file_type & const_share_file_type_ref;

inline bool operator == (const file_type & f1,const file_type & f2){
	return f1.get_ext() == f2.get_ext();
}

}
#endif // !defined(EA_85608C9D_C7A4_4ee8_AD57_C1DDC51C1D9F__INCLUDED_)
