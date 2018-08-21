// OuterProduct.cpp

#include "OuterProduct.h"
#include "InnerProduct.h"
#include "Addition.h"
#include "NumericReal.h"
#include "Vector.h"
#include <string.h>

using namespace GAExpressionLib;

OuterProduct::OuterProduct( void )
{
}

/*virtual*/ OuterProduct::~OuterProduct( void )
{
}

/*virtual*/ bool OuterProduct::IsSameTypeAs( const Operand* operand ) const
{
	if( dynamic_cast< const OuterProduct* >( operand ) )
		return true;

	return false;
}

/*virtual*/ bool OuterProduct::AssociatesWith( const Operation* operation ) const
{
	if( dynamic_cast< const OuterProduct* >( operation ) )
		return true;

	return false;
}

/*virtual*/ bool OuterProduct::DistributesOver( const Operation* operation ) const
{
	if( dynamic_cast< const Addition* >( operation ) )
		return true;

	return false;
}

/*virtual*/ bool OuterProduct::TakesPrecedenceOver( const Operation* operation ) const
{
	if( dynamic_cast< const OuterProduct* >( operation ) )
		return true;

	if( dynamic_cast< const Addition* >( operation ) )
		return true;

	if( dynamic_cast< const InnerProduct* >( operation ) )
		return true;

	return false;
}

/*virtual*/ void OuterProduct::PrintBinaryOperator( char* buffer, int bufferSize, PrintFormat format ) const
{
	switch( format )
	{
		case PRINT_SORT:
		case PRINT_NORMAL:
		{
			strcpy_s( buffer, bufferSize, "^" );
			break;
		}
		case PRINT_LATEX:
		{
			strcpy_s( buffer, bufferSize, "\\wedge" );
			break;
		}
	}
}

/*virtual*/ Operand* OuterProduct::Allocate( void ) const
{
	return new OuterProduct();
}

/*virtual*/ bool OuterProduct::Combine( const Operand* leftOperand, const Operand* rightOperand, Operand** resultOperand ) const
{
	if( Product::Combine( leftOperand, rightOperand, resultOperand ) )
		return true;

	const Vector* leftVector = dynamic_cast< const Vector* >( leftOperand );
	const Vector* rightVector = dynamic_cast< const Vector* >( rightOperand );

	if( leftVector && rightVector && 0 == strcmp( leftVector->name, rightVector->name ) )
	{
		NumericReal* numericReal = new NumericReal();
		numericReal->real = 0.0;
		*resultOperand = numericReal;
	}

	return false;
}

/*virtual*/ bool OuterProduct::Sort( void )
{
	if( IsHomogeneousOfVectors() )
	{
		int adjacentSwapCount = SortByPrint();
		if( adjacentSwapCount > 0 )
		{
			if( adjacentSwapCount % 2 == 1 )
			{
				NumericReal* numericReal = new NumericReal();
				numericReal->real = -1.0;
				operandList.InsertBefore()->data = numericReal;
			}

			return true;
		}
		else
		{
			if( ConsolidateScalars() )
				return true;
		}
	}

	return false;
}

// OuterProduct.cpp