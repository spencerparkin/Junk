// AutoDelete.h

#pragma once

template< typename Type >
class AutoDelete
{
public:

	AutoDelete( Type* type = 0, bool isArray = false );
	~AutoDelete( void );

	void Capture( Type* type, bool isArray = false );
	void Release( void );

private:

	Type* type;
	bool released;
	bool isArray;
};

#include "AutoDelete.hpp"

// AutoDelete.h