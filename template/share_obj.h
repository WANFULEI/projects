#ifndef share_obj_h__
#define share_obj_h__

#include <QHash>

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
inline uint qHash(const share_ptr<T> & obj) throw(){
	return qHash(obj.ptr());
}

#endif // share_obj_h__
