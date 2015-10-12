// Vector.cpp

#include "GACompute.h"

using namespace GACompute;

Vector::Vector( void )
{
	name = 0;
	scalar = 0;
}

/*virtual*/ Vector::~Vector( void )
{
	delete[] name;
	delete scalar;
}

/*virtual*/ void Vector::Print( char* buffer, int bufferSize, PrintFormat printFormat ) const
{
	buffer[0] = '\0';

	if( scalar )
	{
		char subBuffer[ 2048 ];
		scalar->Print( subBuffer, sizeof( subBuffer ), printFormat );

		switch( printFormat )
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

		strcat_s( buffer, bufferSize, subBuffer );
		
		switch( printFormat )
		{
			case PRINT_NORMAL:
			{
				strcat_s( buffer, bufferSize, "]" );
				break;
			}
			case PRINT_LATEX:
			{
				strcat_s( buffer, bufferSize, "\\right]" );
				break;
			}
		}
	}

	switch( printFormat )
	{
		case PRINT_NORMAL:
		{
			strcat_s( buffer, bufferSize, name );
			break;
		}
		case PRINT_LATEX:
		{
			char nameBuffer[ 256 ];
			sprintf_s( nameBuffer, sizeof( nameBuffer ), "\\mathbf{%s}", name );
			strcat_s( buffer, bufferSize, nameBuffer );

			break;
		}
	}
}

/*virtual*/ void Vector::SortKey( char* buffer, int bufferSize ) const
{
	if( name )
		sprintf_s( buffer, bufferSize, "vector_%s", name );
	else
		buffer[0] = '\0';
}

/*virtual*/ Operand* Vector::Clone( void ) const
{
	Vector* vector = new Vector();
	vector->SetName( name );
	if( scalar )
		vector->SetScalar( scalar->Clone() );
	return vector;
}

/*virtual*/ Operand* Vector::Simplify( const SimplificationContext& simplificationContext )
{
	if( !scalar )
		return 0;

	if( scalar->IsMultiplicativeIdentity() )
	{
		SetScalar( 0, true );
		return this;
	}

	SimplificationContext simplificationSubContext;
	simplificationSubContext.target = simplificationContext.target;
	simplificationSubContext.operation = 0;

	if( !Operand::Simplify( scalar, simplificationSubContext ) )
		return 0;

	return this;
}

/*virtual*/ bool Vector::IsScalar( void ) const
{
	return false;
}

/*virtual*/ bool Vector::IsAdditiveIdentity( void ) const
{
	if( !scalar )
		return false;

	return scalar->IsAdditiveIdentity();
}

void Vector::SetName( const char* name )
{
	delete[] this->name;
	int len = strlen( name ) + 1;
	this->name = new char[ len ];
	strcpy_s( this->name, len, name );
}

const char* Vector::GetName( void ) const
{
	return name;
}

void Vector::SetScalar( Operand* scalar, bool deleteExisting /*= true*/ )
{
	if( deleteExisting )
		delete this->scalar;
	this->scalar = scalar;
}

Operand* Vector::GetScalar( void )
{
	return scalar;
}

const Operand* Vector::GetScalar( void ) const
{
	return scalar;
}

void Vector::MarryWithScalar( Operand* scalar )
{
	if( !this->scalar )
		this->scalar = scalar;
	else
	{
		// I guess vectors are polygamists.
		GeometricProduct* geometricProduct = new GeometricProduct();
		geometricProduct->operandList->InsertAfter()->data = this->scalar;
		geometricProduct->operandList->InsertAfter()->data = scalar;
		this->scalar = geometricProduct;
	}
}

// Vector.cpp