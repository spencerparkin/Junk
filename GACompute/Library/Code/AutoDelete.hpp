// AutoDelete.hpp

#include "AutoDelete.h"

template< typename Type >
AutoDelete< Type >::AutoDelete( Type* type /*= 0*/, bool isArray /*= false*/ )
{
	Capture( type, isArray );
}

template< typename Type >
AutoDelete< Type >::~AutoDelete( void )
{
	if( !released )
	{
		if( isArray )
			delete[] type;
		else
			delete type;
	}
}

template< typename Type >
void AutoDelete< Type >::Capture( Type* type, bool isArray /*= false*/ )
{
	this->type = type;
	this->isArray = isArray;
	released = false;
}

template< typename Type >
void AutoDelete< Type >::Release( void )
{
	released = true;
}

// AutoDelete.hpp