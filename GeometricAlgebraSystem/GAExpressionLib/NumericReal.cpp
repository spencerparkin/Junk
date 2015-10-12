// NumericReal.cpp

#include "NumericReal.h"
#include <string.h>
#include <stdio.h>

using namespace GAExpressionLib;

NumericReal::NumericReal( void )
{
	real = 0.0;
}

/*virtual*/ NumericReal::~NumericReal( void )
{
}

/*virtual*/ bool NumericReal::IsSameTypeAs( const Operand* operand ) const
{
	if( dynamic_cast< const NumericReal* >( operand ) )
		return true;

	return false;
}

/*virtual*/ Operand* NumericReal::Clone( void ) const
{
	NumericReal* numericReal = new NumericReal();
	numericReal->real = real;
	return numericReal;
}

/*virtual*/ void NumericReal::Print( char* buffer, int bufferSize, PrintFormat format ) const
{
	switch( format )
	{
		case PRINT_SORT:
		{
			strcpy_s( buffer, bufferSize, "aardvark" );
			break;
		}
		case PRINT_NORMAL:
		case PRINT_LATEX:
		{
			sprintf_s( buffer, bufferSize, "%1.2f", real );
			break;
		}
	}
}

// NumericReal.cpp