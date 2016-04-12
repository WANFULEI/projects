#ifndef BASESET2_H
#define BASESET2_H

#include "baseset2_global.h"
#include <QtCore/QMap>
#include <QtCore/QList>
#include <QtCore/QVector>

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
		QVector<node *> m_nodes; 
	};

	template<typename T,typename M,typename C = QList<T *>>
	class list_vector_manager : public C, public instance<M>
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
	};

	template<typename K,typename V,typename M>
	class map_manager : public QMap<K,V *> , public instance<M>
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

	class BASESET2_EXPORT tmp : public node<tmp>
	{
	public:
		tmp(){}
	};

	inline void test(){
		tmp tp;
		tp.m_nodes[0]->m_nodes[0]->m_nodes;
	}
}

#endif // BASESET2_H
