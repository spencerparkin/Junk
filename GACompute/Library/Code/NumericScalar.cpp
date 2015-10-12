// NumericScalar.cpp

#include "GACompute.h"

using namespace GACompute;

NumericScalar::NumericScalar( void )
{
	realNumber = 0.0;
}

/*virtual*/ NumericScalar::~NumericScalar( void )
{
}

/*virtual*/ void NumericScalar::Print( char* buffer, int bufferSize, PrintFormat printFormat ) const
{
	buffer[0] = '\0';

	switch( printFormat )
	{
		case PRINT_NORMAL:
		case PRINT_LATEX:
		{
			sprintf_s( buffer, bufferSize, "(%1.2f)", realNumber );
			break;
		}
	}
}

/*virtual*/ void NumericScalar::SortKey( char* buffer, int bufferSize ) const
{
	sprintf_s( buffer, bufferSize, "numeric_scalar_%f", realNumber );
}

/*virtual*/ Operand* NumericScalar::Clone( void ) const
{
	NumericScalar* numericScalar = new NumericScalar();
	numericScalar->SetReal( realNumber );
	return numericScalar;
}

/*virtual*/ Operand* NumericScalar::Simplify( const SimplificationContext& simplificationContext )
{
	return 0;
}

/*virtual*/ bool NumericScalar::IsScalar( void ) const
{
	return true;
}

/*virtual*/ bool NumericScalar::IsAdditiveIdentity( void ) const
{
	return( realNumber == 0.0 ) ? true : false;
}

/*virtual*/ bool NumericScalar::IsMultiplicativeIdentity( void ) const
{
	return( realNumber == 1.0 ) ? true : false;
}

void NumericScalar::SetReal( double realNumber )
{
	this->realNumber = realNumber;
}

double NumericScalar::GetReal( void ) const
{
	return realNumber;
}

// NumericScalar.cpp