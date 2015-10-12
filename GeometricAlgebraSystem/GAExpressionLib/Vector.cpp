// Vector.cpp

#include "Vector.h"
#include <string.h>
#include <malloc.h>
#include <stdio.h>

using namespace GAExpressionLib;

Vector::Vector( void )
{
}

/*virtual*/ Vector::~Vector( void )
{
}

/*virtual*/ bool Vector::IsSameTypeAs( const Operand* operand ) const
{
	if( dynamic_cast< const Vector* >( operand ) )
		return true;

	return false;
}

/*virtual*/ Operand* Vector::Clone( void ) const
{
	Vector* vector = new Vector();

	if( name )
		vector->name = _strdup( name );

	if( scalar )
		vector->scalar = scalar->Clone();

	return vector;
}

/*virtual*/ void Vector::Print( char* buffer, int bufferSize, PrintFormat format ) const
{
	buffer[0] = '\0';

	char tempBuffer[ 5*1024 ];

	if( scalar && format != PRINT_SORT )
	{
		switch( format )
		{
			case PRINT_NORMAL:
			{
				strcat_s( buffer, bufferSize, "[" );
				break;
			}
			case PRINT_LATEX:
			{
				strcat_s( buffer, bufferSize, "\\left[" );
				break;
			}
		}

		scalar->Print( tempBuffer, sizeof( tempBuffer ), format );
		strcat_s( buffer, bufferSize, tempBuffer );

		switch( format )
		{
			case PRINT_NORMAL:
			{
				strcat_s( buffer, bufferSize, "]" );
				break;
			}
			case PRINT_LATEX:
			{
				strcat_s( buffer, bufferSize, "\\left]" );
				break;
			}
		}
	}

	tempBuffer[0] = '\0';

	switch( format )
	{
		case PRINT_SORT:
		case PRINT_NORMAL:
		{
			sprintf_s( tempBuffer, sizeof( tempBuffer ), "%s", name );
			break;
		}
		case PRINT_LATEX:
		{
			sprintf_s( tempBuffer, sizeof( tempBuffer ), "\\mathbf{%s}", name );
			break;
		}
	}

	strcat_s( buffer, bufferSize, tempBuffer );
}

// Vector.cpp