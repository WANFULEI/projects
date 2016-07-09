#ifndef manager_h__
#define manager_h__

#include <QList>
#include <QVector>
#include <QMap>
#include "share_obj.h"

template<typename T, typename C = QList<T *> >
class list_vector_manager : public C
{
public:
	virtual ~list_vector_manager(){
		for (int i=0;i<size();++i)
		{
			delete at(i);
		}
		clear();
	}
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
	T * attach_entry(T * p){
		if (p == 0)
		{
			return 0;
		}
		push_back(p);
		return p;
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

	template<typename N>
	QList<N *> get_type_entrys(){
		QList<N *> entries;
		for (int i=0;i<size();++i)
		{
			if (dynamic_cast<N *>(get_entry(i)))
			{
				entries << dynamic_cast<N *>(get_entry(i));
			}
		}
		return entries;
	}
};

template<typename T, typename C = QList<share_ptr<T> > >
class share_list_vector_manager : public C
{
public:
	~share_list_vector_manager(){
		clear();
	}
	share_ptr<T> create_entry(){
		share_ptr<T> p = new T;
		push_back(p);
		return p;
	}
	template<typename M>
	share_ptr<T> create_entry(){
		share_ptr<T> p = new M;
		push_back(p);
		return p;
	}
	bool delete_entry(const share_ptr<T> & p){
		return removeOne(p);
	}
	share_ptr<T> attach_entry(const share_ptr<T> & p){
		if (p == 0)
		{
			return share_ptr<T>();
		}
		push_back(p);
		return p;
	}
	share_ptr<T> detach_entry(const share_ptr<T> & p){
		share_ptr<T> res(p);
		removeOne(p);
		return res;
	}

	share_ptr<T> get_entry(int i){
		if (i < 0 || i >= size())
		{
			return share_ptr<T>();
		}
		return at(i);
	}

	template<typename N>
	QList<share_ptr<N> > get_type_entrys(){
		QList<share_ptr<N> > entries;
		for (int i=0;i<size();++i)
		{
			if (dynamic_cast<N *>(get_entry(i)))
			{
				entries << share_ptr<N>(dynamic_cast<N *>(get_entry(i)));
			}
		}
		return entries;
	}
};

template<typename K, typename V>
class map_manager : public QMap<K,V *>
{
public:
	V * create_entry(const K & k){
		auto iter = find(k);
		if (iter != end())
		{
			delete iter.value();
		}
		V * p = new V;
		insert(k,p);
		return p;
	}
	bool delete_entry(const K & k){
		auto iter = find(k);
		if (iter != end())
		{
			delete iter.value();
			erase(iter);
			return true;
		}
		return false;
	}
	V * attach_entry(const K & k,V * p){
		if (p == 0)
		{
			return 0;
		}
		auto iter = find(k);
		if (iter != end())
		{
			delete iter.value();
		}
		insert(k,p);
		return p;
	}
	V * detach_entry(const K & k){
		auto iter = find(k);
		if (iter != end())
		{
			V * p = iter.value();
			erase(iter);
			return p;
		}
		return 0;
	}
	V * get_entry(const K & k){
		auto iter = find(k);
		if (iter != end())
		{
			return iter.value();
		}
		return 0;
	}

};

template<typename K, typename V>
class share_map_manager : public QMap<K,share_ptr<V> >
{
public:
	share_ptr<V> create_entry(const K & k){
		share_ptr<V> p = new V;
		insert(k,p);
		return p;
	}
	bool delete_entry(const K & k){
		auto iter = find(k);
		if (iter != end())
		{
			erase(iter);
			return true;
		}
		return false;
	}
	share_ptr<V> attach_entry(const K & k,const share_ptr<V> & p){
		if (p.is_null())
		{
			return share_ptr<V>();
		}
		insert(k,p);
		return p;
	}
	share_ptr<V> detach_entry(const K & k){
		auto iter = find(k);
		if (iter != end())
		{
			share_ptr<V> p = iter.value();
			erase(iter);
			return p;
		}
		return share_ptr<V>();
	}
	share_ptr<V> get_entry(const K & k){
		auto iter = find(k);
		if (iter != end())
		{
			return iter.value();
		}
		return share_ptr<V>();
	}

};
#endif // manager_h__
