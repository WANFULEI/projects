#ifndef BASESET2_H
#define BASESET2_H

#include "baseset2_global.h"

namespace baseset
{
	template<typename T>
	class list_manager : public QList<T *>
	{
	public:
		T * create_entry(){
			T * p = new T;
			push_back(p);
			return p;
		}
		bool delete_entry(T * p){
			if (removeOne(p))
			{
				delete p;
				return true;
			}
			return false;
		}
		void attach_entry(T * p){
			push_back(p);
		}
		T * detach_entry(T * p){
			removeOne(p);
			return p;
		}

		T * get_entry(int i){
			if (i < 0 || i >= size())
			{
				return 0;
			}
			return at(i);
		}
	};

	class BASESET2_EXPORT tmp{};
}

#endif // BASESET2_H
