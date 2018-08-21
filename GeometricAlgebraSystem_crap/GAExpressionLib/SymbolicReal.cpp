// SymbolicReal.cpp

#include "SymbolicReal.h"
#include <string.h>
#include <malloc.h>
#include <stdio.h>

using namespace GAExpressionLib;

SymbolicReal::SymbolicReal( void )
{
	exponent = 1;
}

/*virtual*/ SymbolicReal::~SymbolicReal( void )
{
}

/*virtual*/ bool SymbolicReal::IsSameTypeAs( const Operand* operand ) const
{
	if( dynamic_cast< const SymbolicReal* >( operand ) )
		return true;

	return false;
}

/*virtual*/ Operand* SymbolicReal::Clone( void ) const
{
	SymbolicReal* symbolicReal = new SymbolicReal();

	if( name )
		symbolicReal->name = _strdup( name );

	if( scalar )
		symbolicReal->scalar = scalar->Clone();

	return symbolicReal;
}

/*virtual*/ void SymbolicReal::Print( char* buffer, int bufferSize, PrintFormat format ) const
{
	buffer[0] = '\0';

	char tempBuffer[ 5*1024 ];

	if( scalar && format != PRINT_SORT )
	{
		switch( format )
		{
			case PRINT_NORMAL:
			{
				strcat_s( buffer, bufferSize, "{" );
				break;
			}
			case PRINT_LATEX:
			{
				strcat_s( buffer, bufferSize, "\\left\\{" );
				break;
			}
		}

		scalar->Print( tempBuffer, sizeof( tempBuffer ), format );
		strcat_s( buffer, bufferSize, tempBuffer );

		switch( format )
		{
			case PRINT_NORMAL:
			{
				strcat_s( buffer, bufferSize, "}" );
				break;
			}
			case PRINT_LATEX:
			{
				strcat_s( buffer, bufferSize, "\\left\\}" );
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
			if( exponent == 1 )
				sprintf_s( tempBuffer, sizeof( tempBuffer ), "$%s", name );
			else
				sprintf_s( tempBuffer, sizeof( tempBuffer ), "$%s^%d", name, exponent );
			break;
		}
		case PRINT_LATEX:
		{
			if( exponent == 1 )
				sprintf_s( tempBuffer, sizeof( tempBuffer ), "\\mathbb{%s}", name );
			else
				sprintf_s( tempBuffer, sizeof( tempBuffer ), "\\mathbb{%s}^{%d}", name, exponent );
			break;
		}
	}

	strcat_s( buffer, bufferSize, tempBuffer );
}

// SymbolicReal.cpp