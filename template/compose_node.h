/*!
 * \file compose_node.h
 * \date 2016/06/25 13:05
 *
 * \author ÎºÇå
 * Contact: 897810981@qq.com
 *
 * \brief 
 *
 * TODO: long description
 *
 * \note
*/

#ifndef compose_node_h__
#define compose_node_h__

#include <QList>

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
#endif // compose_node_h__
