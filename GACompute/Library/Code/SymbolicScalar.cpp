// SymbolicScalar.cpp

#include "GACompute.h"

using namespace GACompute;

SymbolicScalar::SymbolicScalar( void )
{
	name = 0;
	exponent = 1;
}

/*virtual*/ SymbolicScalar::~SymbolicScalar( void )
{
	delete[] name;
}

/*virtual*/ void SymbolicScalar::Print( char* buffer, int bufferSize, PrintFormat printFormat ) const
{
	buffer[0] = '\0';

	switch( printFormat )
	{
		case PRINT_NORMAL:
		{
			if( exponent == 1 )
				sprintf_s( buffer, bufferSize, "(%s)", name );
			else
				sprintf_s( buffer, bufferSize, "(%s^%d)", name, exponent );
			break;
		}
		case PRINT_LATEX:
		{
			if( exponent == 1 )
				sprintf_s( buffer, bufferSize, "(%s)", name );
			else
				sprintf_s( buffer, bufferSize, "(%s)^{%d}", name, exponent );
			break;
		}
	}
}

/*virtual*/ void SymbolicScalar::SortKey( char* buffer, int bufferSize ) const
{
	if( name )
		sprintf_s( buffer, bufferSize, "symbolic_scalar_%s", name );
	else
		buffer[0] = '\0';
}

/*virtual*/ Operand* SymbolicScalar::Clone( void ) const
{
	SymbolicScalar* symbolicScalar = new SymbolicScalar();
	symbolicScalar->SetName( name );
	symbolicScalar->SetExponent( exponent );
	return symbolicScalar;
}

/*virtual*/ Operand* SymbolicScalar::Simplify( const SimplificationContext& simplificationContext )
{
	if( exponent == 0 )
	{
		NumericScalar* numericScalar = new NumericScalar();
		numericScalar->SetReal( 1.0 );
		return numericScalar;
	}

	// TODO: It might be a good idea to query a variable subsitutor here.
	//       It may be useful to copy a simbolic expression, then evaluate the copy with a subsitutor registered.

	return 0;
}

/*virtual*/ bool SymbolicScalar::IsScalar( void ) const
{
	return true;
}

/*virtual*/ bool SymbolicScalar::IsAdditiveIdentity( void ) const
{
	return false;
}

/*virtual*/ bool SymbolicScalar::IsMultiplicativeIdentity( void ) const
{
	return( exponent == 0 ? true : false );
}

void SymbolicScalar::SetName( const char* name )
{
	delete[] this->name;
	int len = strlen( name ) + 1;
	this->name = new char[ len ];
	strcpy_s( this->name, len, name );
}

const char* SymbolicScalar::GetName( void ) const
{
	return name;
}

void SymbolicScalar::SetExponent( int exponent )
{
	this->exponent = exponent;
}

int SymbolicScalar::GetExponent( void ) const
{
	return exponent;
}

// SymbolicScalar.cpp