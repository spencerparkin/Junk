// GeometricProduct.cpp

#include "GeometricProduct.h"
#include "Addition.h"
#include "InnerProduct.h"
#include "OuterProduct.h"
#include "Vector.h"
#include "BilinearForm.h"
#include <string.h>

using namespace GAExpressionLib;

GeometricProduct::GeometricProduct( void )
{
}

/*virtual*/ GeometricProduct::~GeometricProduct( void )
{
}

/*virtual*/ bool GeometricProduct::IsSameTypeAs( const Operand* operand ) const
{
	if( dynamic_cast< const GeometricProduct* >( operand ) )
		return true;

	return false;
}

/*virtual*/ bool GeometricProduct::AssociatesWith( const Operation* operation ) const
{
	if( dynamic_cast< const GeometricProduct* >( operation ) )
		return true;

	return false;
}

/*virtual*/ bool GeometricProduct::DistributesOver( const Operation* operation ) const
{
	if( dynamic_cast< const Addition* >( operation ) )
		return true;

	return false;
}

/*virtual*/ bool GeometricProduct::TakesPrecedenceOver( const Operation* operation ) const
{
	if( dynamic_cast< const GeometricProduct* >( operation ) )
		return true;

	if( dynamic_cast< const Addition* >( operation ) )
		return true;

	if( dynamic_cast< const InnerProduct* >( operation ) )
		return true;

	if( dynamic_cast< const OuterProduct* >( operation ) )
		return true;

	return false;
}

/*virtual*/ void GeometricProduct::PrintBinaryOperator( char* buffer, int bufferSize, PrintFormat format ) const
{
	switch( format )
	{
		case PRINT_SORT:
		case PRINT_NORMAL:
		{
			strcpy_s( buffer, bufferSize, "*" );
			break;
		}
		case PRINT_LATEX:
		{
			strcpy_s( buffer, bufferSize, "" );		// Juxtaposition, but might be confusing if we have all numeric operands.
			break;
		}
	}
}

/*virtual*/ Operand* GeometricProduct::Allocate( void ) const
{
	return new GeometricProduct();
}

/*virtual*/ bool GeometricProduct::Combine( const Operand* leftOperand, const Operand* rightOperand, Operand** resultOperand ) const
{
	if( Product::Combine( leftOperand, rightOperand, resultOperand ) )
		return true;

	const Vector* leftVector = dynamic_cast< const Vector* >( leftOperand );
	const Vector* rightVector = dynamic_cast< const Vector* >( rightOperand );

	if( leftVector && rightVector && 0 == strcmp( leftVector->name, rightVector->name ) )
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

/*virtual*/ bool GeometricProduct::Sort( void )
{
	if( IsHomogeneousOfScalars() )
	{
		if( Operation::Sort() )
			return true;
		else if( ConsolidateScalars() )
			return true;
	}
	else if( IsHomogeneousOfVectors() )
	{
		if( ConsolidateScalars() )
			return true;
	}

	return false;
}

// GeometricProduct.cpp