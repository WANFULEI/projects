#ifndef OBJECTFACTORY_H
#define OBJECTFACTORY_H

#include <QByteArray>
#include <QMetaObject>
#include <QHash>

class CObjectFactory
{
public:
	template<typename T>
	static void registerClass()
	{
		constructors().insert( T::staticMetaObject.className(), &constructorHelper<T> );
	}

	static QObject* createObject( const QByteArray& className, QObject* parent = NULL )
	{
		Constructor constructor = constructors().value( className );
		if ( constructor == 0 )
			return 0;
		return (*constructor)( parent );
	}

private:
	typedef QObject* (*Constructor)( QObject* parent );

	template<typename T>
	static QObject* constructorHelper( QObject* parent )
	{
		return new T( parent );
	}

	static QHash<QByteArray, Constructor>& constructors()
	{
		static QHash<QByteArray, Constructor> instance;
		return instance;
	}
};

#endif // OBJECTFACTORY_H