// InnerProduct.cpp

#include "InnerProduct.h"
#include "Addition.h"
#include "Vector.h"
#include "BilinearForm.h"
#include <string.h>

using namespace GAExpressionLib;

InnerProduct::InnerProduct( void )
{
}

/*virtual*/ InnerProduct::~InnerProduct( void )
{
}

/*virtual*/ bool InnerProduct::IsSameTypeAs( const Operand* operand ) const
{
	if( dynamic_cast< const InnerProduct* >( operand ) )
		return true;

	return false;
}

/*virtual*/ bool InnerProduct::AssociatesWith( const Operation* operation ) const
{
	return false;
}

/*virtual*/ bool InnerProduct::DistributesOver( const Operation* operation ) const
{
	if( dynamic_cast< const Addition* >( operation ) )
		return true;

	return false;
}

/*virtual*/ bool InnerProduct::TakesPrecedenceOver( const Operation* operation ) const
{
	if( dynamic_cast< const InnerProduct* >( operation ) )
		return true;

	if( dynamic_cast< const Addition* >( operation ) )
		return true;

	return false;
}

/*virtual*/ void InnerProduct::PrintBinaryOperator( char* buffer, int bufferSize, PrintFormat format ) const
{
	switch( format )
	{
		case PRINT_SORT:
		case PRINT_NORMAL:
		{
			strcpy_s( buffer, bufferSize, "." );
			break;
		}
		case PRINT_LATEX:
		{
			strcpy_s( buffer, bufferSize, "\\cdot" );
			break;
		}
	}
}

/*virtual*/ Operand* InnerProduct::Allocate( void ) const
{
	return new InnerProduct();
}

// STPTODO: It may be that we never should have recognized an inner product as a scalar,
//          and that we should have created an InnerProductOfTwoVectors class.

/*virtual*/ bool InnerProduct::IsScalar( void ) const
{
	int vectorCount = 0;
	int scalarCount = 0;

	for( const OperandList::Node* node = operandList.Head(); node; node = node->Next() )
	{
		if( node->data->IsScalar() )
			scalarCount++;

		const Vector* vector = dynamic_cast< const Vector* >( node->data );
		if( vector )
			vectorCount++;
	}

	if( scalarCount + vectorCount != operandList.Count() )
		return false;

	if( vectorCount % 2 == 0 )
		return true;

	// We may still be a scalar, but we shouldn't need to detect those other cases.
	return false;
}

/*virtual*/ bool InnerProduct::Combine( const Operand* leftOperand, const Operand* rightOperand, Operand** resultOperand ) const
{
	if( Product::Combine( leftOperand, rightOperand, resultOperand ) )
		return true;

	const Vector* leftVector = dynamic_cast< const Vector* >( leftOperand );
	const Vector* rightVector = dynamic_cast< const Vector* >( rightOperand );

	if( leftVector && rightVector )
	{
		if( bilinearForm )
		{
			Operand* operand = bilinearForm->Evaluate( leftVector, rightVector );
			if( operand )
			{
				*resultOperand = operand;
				return true;
			}
		}
	}

	return false;
}

/*virtual*/ bool InnerProduct::Sort( void )
{
	if( operandList.Count() == 2 )
	{
		const Vector* leftVector = dynamic_cast< const Vector* >( operandList.Head()->data );
		const Vector* rightVector = dynamic_cast< const Vector* >( operandList.Tail()->data );

		if( leftVector && rightVector )
		{
			if( Operation::Sort() )
				return true;
			else if( ConsolidateScalars() )
				return true;
		}
	}

	return false;
}

// InnerProduct.cpp