#ifndef singleton_h__
#define singleton_h__


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

#endif // singleton_h__