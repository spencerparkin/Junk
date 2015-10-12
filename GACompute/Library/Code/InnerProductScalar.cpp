// InnerProductScalar.cpp

#include "GACompute.h"

using namespace GACompute;

InnerProductScalar::InnerProductScalar( void )
{
	nameA = 0;
	nameB = 0;

	exponent = 1;
}

/*virtual*/ InnerProductScalar::~InnerProductScalar( void )
{
	delete[] nameA;
	delete[] nameB;
}

/*virtual*/ void InnerProductScalar::Print( char* buffer, int bufferSize, PrintFormat printFormat ) const
{
	buffer[0] = '\0';

	switch( printFormat )
	{
		case PRINT_NORMAL:
		{
			if( exponent == 1 )
				sprintf_s( buffer, bufferSize, "(%s.%s)", nameA, nameB );
			else
				sprintf_s( buffer, bufferSize, "((%s.%s)^%d)", nameA, nameB, exponent );
			break;
		}
		case PRINT_LATEX:
		{
			if( exponent == 1.0 )
				sprintf_s( buffer, bufferSize, "\\left(\\mathbf{%s}\\cdot\\mathbf{%s}\\right)", nameA, nameB );
			else
				sprintf_s( buffer, bufferSize, "\\left(\\mathbf{%s}\\cdot\\mathbf{%s}\\right)^{%d}", nameA, nameB, exponent );
			break;
		}
	}
}

/*virtual*/ void InnerProductScalar::SortKey( char* buffer, int bufferSize ) const
{
	if( nameA && nameB )
		sprintf_s( buffer, bufferSize, "inner_product_scalar_%s_%s", nameA, nameB );
	else
		buffer[0] = '\0';
}

/*virtual*/ Operand* InnerProductScalar::Clone( void ) const
{
	InnerProductScalar* innerProductScalar = new InnerProductScalar();
	innerProductScalar->SetNames( nameA, nameB );
	innerProductScalar->SetExponent( exponent );
	return innerProductScalar;
}

/*virtual*/ Operand* InnerProductScalar::Simplify( const SimplificationContext& simplificationContext )
{
	if( exponent == 0 )
	{
		NumericScalar* numericScalar = new NumericScalar();
		numericScalar->SetReal( 1.0 );
		return numericScalar;
	}

	BilinearForm* bilinearForm = BilinearForm::Get();
	if( !bilinearForm )
		return 0;

	double result;
	if( !bilinearForm->Evaluate( nameA, nameB, result ) )
		return 0;

	NumericScalar* numericScalar = new NumericScalar();
	numericScalar->SetReal( result );
	return numericScalar;
}

/*virtual*/ bool InnerProductScalar::IsScalar( void ) const
{
	return true;
}

/*virtual*/ bool InnerProductScalar::IsAdditiveIdentity( void ) const
{
	return false;
}

/*virtual*/ bool InnerProductScalar::IsMultiplicativeIdentity( void ) const
{
	return( exponent == 0 ? true : false );
}

void InnerProductScalar::SetNameA( const char* nameA )
{
	delete[] this->nameA;
	int len = strlen( nameA ) + 1;
	this->nameA = new char[ len ];
	strcpy_s( this->nameA, len, nameA );
}

void InnerProductScalar::SetNameB( const char* nameB )
{
	delete[] this->nameB;
	int len = strlen( nameB ) + 1;
	this->nameB = new char[ len ];
	strcpy_s( this->nameB, len, nameB );
}

void InnerProductScalar::SetNames( const char* nameA, const char* nameB )
{
	if( strcmp( nameA, nameB ) < 0 )
	{
		SetNameA( nameA );
		SetNameB( nameB );
	}
	else
	{
		SetNameA( nameB );
		SetNameB( nameA );
	}
}

const char* InnerProductScalar::GetNameA( void ) const
{
	return nameA;
}

const char* InnerProductScalar::GetNameB( void ) const
{
	return nameB;
}

void InnerProductScalar::SetExponent( int exponent )
{
	this->exponent = exponent;
}

int InnerProductScalar::GetExponent( void ) const
{
	return exponent;
}

// InnerProductScalar.cpp