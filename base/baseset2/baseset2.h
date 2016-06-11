#ifndef BASESET2_H
#define BASESET2_H

#include "baseset2_global.h"
#include <QtCore/QMap>
#include <QtCore/QList>
#include <QtCore/QVector>
#ifdef _WIN32
#include "windows.h"
#endif

namespace baseset
{
	template<typename T>
	class instance
	{
	public:
		static T * get_instance(){
			static T m;
			return &m;
		}
	};

	template<typename T>
	class instance2
	{
	public:
		static T * get_instance(){
			static T * m = 0;
			if (m == 0)
			{
				m = new T;
			}
			return m;
		}
	};

	class share_obj
	{
	private:
		int _ref_count;
	public:
		share_obj(){
			_ref_count = 0;
		}
		~share_obj(){
		}
		int ref(){
			return ++_ref_count;
		}
		int unref(){
			return --_ref_count;
		}
		int ref_count(){
			return _ref_count;
		}
	};

	template<typename T>
	class share_ptr
	{
	private:
		T * _ptr;
	public:
		share_ptr(){ _ptr = 0; }
		share_ptr(T * ptr){
			ref_new(ptr);
		}
		share_ptr(const share_ptr<T> & obj){
			ref_new(obj._ptr);
		}
		~share_ptr(){
			unref_this();
		}
		const share_ptr<T> & operator = (const share_ptr<T> & obj){
			unref_this();
			ref_new(obj._ptr);
			return *this;
		}
		T * ptr() const{
			return _ptr;
		}
		bool is_null() const{
			return _ptr == 0;
		}
		T * operator -> () const{
			return _ptr;
		}
		T & operator * () const{
			return *_ptr;
		}
		operator T * () const{
			return _ptr;
		}

	private:
		void unref_this(){
			if(_ptr)
			{
				if (_ptr->unref() == 0)
				{
					delete _ptr;
				}
			}
		}
		void ref_new(T * ptr){
			_ptr = ptr;
			if (ptr)
			{
				ptr->ref();
			}
		}
	};

	template<typename T>
	bool operator == (const share_ptr<T> & ptr1,const share_ptr<T> & ptr2){
		return ptr1.ptr() == ptr2.ptr();
	}


	template<typename T>
	bool operator != (const share_ptr<T> & ptr1,const share_ptr<T> & ptr2){
		return !(ptr1 == ptr2);
	}

	template<typename T>
	uint qHash(const share_ptr<T> & obj) throw(){
		return qHash(obj.ptr());
	}

	template<typename node>
	class node
	{
	public:
		virtual ~node(){
			for (int i=0;i<m_nodes.size();++i)
			{
				delete m_nodes[i];
			}
			m_nodes.clear();
		}
		node * create_node(){
			node * p = new node;
			m_nodes.push_back(p);
			return p;
		}
		bool delete_node(node * p){
			if (p == 0)
			{
				return false;
			}
			if (m_nodes.removeOne(p))
			{
				delete p;
				return true;
			}
			return false;
		}
		bool attach_node(node * p){
			if (p == 0)
			{
				return false;
			}
			m_nodes << p;
			return true;
		}
		node * detach_node(node * p){
			m_nodes.removeOne(p);
			return p;
		}
		node * get_node(int i){
			if (i < 0 || i >= m_nodes.size())
			{
				return 0;
			}
			return m_nodes[i];
		}

	public:
		QList<node *> m_nodes; 
	};

	template<typename T,typename C = QList<T *> >
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

	template<typename T,typename C = QList<share_ptr<T> > >
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

	template<typename K,typename V>
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

	template<typename K,typename V>
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


	class BASESET2_EXPORT tmp : public node<tmp>
	{
	public:
		tmp(){}
	};

	inline void test(){
		tmp tp;
		tp.m_nodes[0]->m_nodes[0]->m_nodes;
	}

#ifdef _WIN32
	class time_elapsed
	{
	public:
		time_elapsed(){ QueryPerformanceFrequency(&m_start_freq); start(); }
		void start(){ QueryPerformanceCounter(&m_start_time); }
		int stop(){
			LARGE_INTEGER end_time;
			QueryPerformanceCounter(&end_time);
			return(((end_time.QuadPart - m_start_time.QuadPart) * 1e6) 
				/ m_start_freq.QuadPart);
		}

	private:
		LARGE_INTEGER m_start_freq;
		LARGE_INTEGER m_start_time;
	};
#else
#endif


}

#endif // BASESET2_H
