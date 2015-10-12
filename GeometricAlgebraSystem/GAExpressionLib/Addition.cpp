// Addition.cpp

#include "Addition.h"
#include "NumericReal.h"
#include "Product.h"
#include "GeometricProduct.h"
#include "ScaledSymbol.h"
#include "Vector.h"
#include "SymbolicReal.h"
#include <string.h>

using namespace GAExpressionLib;

Addition::Addition( void )
{
}

/*virtual*/ Addition::~Addition( void )
{
}

/*virtual*/ bool Addition::IsSameTypeAs( const Operand* operand ) const
{
	if( dynamic_cast< const Addition* >( operand ) )
		return true;

	return false;
}

/*virtual*/ bool Addition::Commutes( void ) const
{
	return true;
}

/*virtual*/ void Addition::PrintIdentity( char* buffer, int bufferSize, PrintFormat format ) const
{
	strcpy_s( buffer, bufferSize, "<1>" );
}

/*virtual*/ bool Addition::AssociatesWith( const Operation* operation ) const
{
	if( dynamic_cast< const Addition* >( operation ) )
		return true;

	return false;
}

/*virtual*/ bool Addition::DistributesOver( const Operation* operation ) const
{
	return false;
}

/*virtual*/ bool Addition::TakesPrecedenceOver( const Operation* operation ) const
{
	if( dynamic_cast< const Addition* >( operation ) )
		return true;

	return false;
}

/*virtual*/ void Addition::PrintBinaryOperator( char* buffer, int bufferSize, PrintFormat format ) const
{
	strcpy_s( buffer, bufferSize, " + " );
}

/*virtual*/ Operand* Addition::Identity( void ) const
{
	NumericReal* numericReal = new NumericReal();
	numericReal->real = 0.0;
	return numericReal;
}

/*virtual*/ Operand* Addition::Allocate( void ) const
{
	return new Addition();
}

/*virtual*/ bool Addition::Combine( const Operand* leftOperand, const Operand* rightOperand, Operand** resultOperand ) const
{
	const NumericReal* leftNumericReal = dynamic_cast< const NumericReal* >( leftOperand );
	const NumericReal* rightNumericReal = dynamic_cast< const NumericReal* >( rightOperand );

	if( leftNumericReal && leftNumericReal->real == 0.0 )
	{
		*resultOperand = rightOperand->Clone();
		return true;
	}

	if( rightNumericReal && rightNumericReal->real == 0.0 )
	{
		*resultOperand = leftOperand->Clone();
		return true;
	}

	if( leftNumericReal && rightNumericReal )
	{
		NumericReal* numericReal = new NumericReal();
		numericReal->real = leftNumericReal->real + rightNumericReal->real;
		*resultOperand = numericReal;
		return true;
	}

	const Vector* leftVector = dynamic_cast< const Vector* >( leftOperand );
	const Vector* rightVector = dynamic_cast< const Vector* >( rightOperand );

	if( leftVector && rightVector && 0 == strcmp( leftVector->name, rightVector->name ) )
	{
		Addition* addition = new Addition();

		if( leftVector->scalar )
			addition->operandList.InsertAfter()->data = leftVector->scalar->Clone();
		if( rightVector->scalar )
			addition->operandList.InsertAfter()->data = rightVector->scalar->Clone();

		Vector* vector = new Vector();
		vector->name = _strdup( leftVector->name );

		if( addition->operandList.Count() > 0 )
			vector->scalar = addition;
		else
			delete addition;

		return true;
	}

	const SymbolicReal* leftSymbolicReal = dynamic_cast< const SymbolicReal* >( leftOperand );
	const SymbolicReal* rightSymbolicReal = dynamic_cast< const SymbolicReal* >( rightOperand );

	if( leftSymbolicReal && rightSymbolicReal && 0 == strcmp( leftSymbolicReal->name, rightSymbolicReal->name ) )
	{
		Addition* addition = new Addition();

		if( leftSymbolicReal->scalar )
			addition->operandList.InsertAfter()->data = leftSymbolicReal->scalar->Clone();
		if( rightSymbolicReal->scalar )
			addition->operandList.InsertAfter()->data = rightSymbolicReal->scalar->Clone();

		SymbolicReal* symbolicReal = new SymbolicReal();
		symbolicReal->name = _strdup( leftSymbolicReal->name );

		if( addition->operandList.Count() > 0 )
			symbolicReal->scalar = addition;
		else
			delete addition;

		return true;
	}

	const Product* leftProduct = dynamic_cast< const Product* >( leftOperand );
	const Product* rightProduct = dynamic_cast< const Product* >( rightOperand );

	if( leftProduct && rightProduct && leftProduct->operandList.Count() == rightProduct->operandList.Count() )
	{
		if( leftProduct->IsSameTypeAs( rightProduct ) )
		{
			if( ( leftProduct->IsHomogeneousOfVectors() && rightProduct->IsHomogeneousOfVectors() ) ||
				( leftProduct->IsHomogeneousOfSymbolicReals() && rightProduct->IsHomogeneousOfSymbolicReals() ) )
			{
				const OperandList::Node* leftNode = leftProduct->operandList.Head();
				const OperandList::Node* rightNode = rightProduct->operandList.Head();

				// We can identify like-terms here this way, because we sort all products before-hand.
				while( leftNode && rightNode )
				{
					const Operand* leftOperand = leftNode->data;
					const Operand* rightOperand = rightNode->data;

					const ScaledSymbol* leftScaledSymbol = dynamic_cast< const ScaledSymbol* >( leftOperand );
					const ScaledSymbol* rightScaledSymbol = dynamic_cast< const ScaledSymbol* >( rightNode->data );

					if( 0 != strcmp( leftScaledSymbol->name, rightScaledSymbol->name ) )
						break;

					leftNode = leftNode->Next();
					rightNode = rightNode->Next();
				}

				if( !( leftNode && rightNode ) )
				{
					Product* leftProductClone = ( Product* )leftProduct->Clone();
					GeometricProduct* leftGeometricProduct = new GeometricProduct();
					leftProductClone->StripScalars( leftGeometricProduct->operandList );

					GeometricProduct* rightGeometricProduct = new GeometricProduct();
					rightProduct->CloneScalars( rightGeometricProduct->operandList );
				
					Addition* addition = new Addition();
					addition->operandList.InsertAfter()->data = leftGeometricProduct;
					addition->operandList.InsertAfter()->data = rightGeometricProduct;

					ScaledSymbol* scaledSymbol = leftProductClone->FindLeadingScaledSymbol();
					scaledSymbol->scalar = addition;
					*resultOperand = leftProductClone;

					return true;
				}
			}
		}
	}

	return false;
}

// Addition.cpp