// Product.cpp

#include "Product.h"
#include "NumericReal.h"
#include "SymbolicReal.h"
#include "Scalar.h"
#include "Vector.h"
#include "GeometricProduct.h"
#include "OuterProduct.h"
#include <string.h>

using namespace GAExpressionLib;

Product::Product( void )
{
}

/*virtual*/ Product::~Product( void )
{
}

/*virtual*/ bool Product::Commutes( void ) const
{
	// Generally, the products don't commute.  Under some circumstances they do,
	// and yet under other they anti-commute, but generally they don't commute.
	return false;
}

/*virtual*/ void Product::PrintIdentity( char* buffer, int bufferSize, PrintFormat format ) const
{
	strcpy_s( buffer, bufferSize, "<1>" );
}
	
/*virtual*/ Operand* Product::Identity( void ) const
{
	NumericReal* numericReal = new NumericReal();
	numericReal->real = 1.0;
	return numericReal;
}

/*virtual*/ bool Product::Combine( const Operand* leftOperand, const Operand* rightOperand, Operand** resultOperand ) const
{
	const NumericReal* leftNumericReal = dynamic_cast< const NumericReal* >( leftOperand );
	const NumericReal* rightNumericReal = dynamic_cast< const NumericReal* >( rightOperand );

	if( leftNumericReal )
	{
		if( leftNumericReal->real == 0.0 )
		{
			*resultOperand = leftNumericReal->Clone();
			return true;
		}
		else if( leftNumericReal->real == 1.0 )
		{
			*resultOperand = rightOperand->Clone();
			return true;
		}
	}

	if( rightNumericReal )
	{
		if( rightNumericReal->real == 0.0 )
		{
			*resultOperand = rightNumericReal->Clone();
			return true;
		}
		else if( rightNumericReal->real == 1.0 )
		{
			*resultOperand = leftOperand->Clone();
			return true;
		}
	}

	if( leftNumericReal && rightNumericReal )
	{
		NumericReal* numericReal = new NumericReal();
		numericReal->real = leftNumericReal->real * rightNumericReal->real;
		return true;
	}

	const SymbolicReal* leftSymbolicReal = dynamic_cast< const SymbolicReal* >( leftOperand );
	const SymbolicReal* rightSymbolicReal = dynamic_cast< const SymbolicReal* >( rightOperand );

	if( leftNumericReal && rightSymbolicReal )
	{
		SymbolicReal* symbolicReal = ( SymbolicReal* )rightSymbolicReal->Clone();

		if( !symbolicReal->scalar )
			symbolicReal->scalar = leftNumericReal->Clone();
		else
		{
			GeometricProduct* geometricProduct = new GeometricProduct();
			geometricProduct->operandList.InsertAfter()->data = leftNumericReal->Clone();
			geometricProduct->operandList.InsertAfter()->data = symbolicReal->scalar;

			symbolicReal->scalar = geometricProduct;
		}
	}

	if( leftSymbolicReal && rightNumericReal )
	{
		SymbolicReal* symbolicReal = ( SymbolicReal* )leftSymbolicReal->Clone();

		if( !symbolicReal->scalar )
			symbolicReal->scalar = rightNumericReal->Clone();
		else
		{
			GeometricProduct* geometricProduct = new GeometricProduct();
			geometricProduct->operandList.InsertAfter()->data = symbolicReal->scalar;
			geometricProduct->operandList.InsertAfter()->data = rightNumericReal->Clone();

			symbolicReal->scalar = geometricProduct;
		}
	}

	if( leftSymbolicReal && rightSymbolicReal && 0 == strcmp( leftSymbolicReal->name, rightSymbolicReal->name ) )
	{
		Operand* leftScalar = leftSymbolicReal->scalar;
		Operand* rightScalar = rightSymbolicReal->scalar;

		Operand* scalar = nullptr;

		if( leftScalar && rightScalar )
		{
			GeometricProduct* geometricProduct = new GeometricProduct();
			geometricProduct->operandList.InsertAfter()->data = leftScalar->Clone();
			geometricProduct->operandList.InsertAfter()->data = rightScalar->Clone();

			scalar = geometricProduct;
		}
		else if( leftScalar )
			scalar = leftScalar->Clone();
		else if( rightScalar )
			scalar = rightScalar->Clone();

		int exponent = leftSymbolicReal->exponent + rightSymbolicReal->exponent;
		if( exponent == 0 )
		{
			if( !scalar )
			{
				NumericReal* numericReal = new NumericReal();
				numericReal->real = 1.0;
				*resultOperand = numericReal;
				return true;
			}
			else
			{
				*resultOperand = scalar;
				return true;
			}
		}
		else
		{
			SymbolicReal* symbolicReal = new SymbolicReal();
			symbolicReal->name = _strdup( leftSymbolicReal->name );
			symbolicReal->exponent = exponent;
			symbolicReal->scalar = scalar;

			*resultOperand = symbolicReal;
			return true;
		}
	}

	const Vector* leftVector = dynamic_cast< const Vector* >( leftOperand );
	const Vector* rightVector = dynamic_cast< const Vector* >( rightOperand );

	if( leftOperand->IsScalar() && rightVector )
	{
		Vector* vector = ( Vector* )rightVector->Clone();

		if( !vector->scalar )
			vector->scalar = leftOperand->Clone();
		else
		{
			GeometricProduct* geometricProduct = new GeometricProduct();
			geometricProduct->operandList.InsertAfter()->data = leftOperand->Clone();
			geometricProduct->operandList.InsertAfter()->data = vector->scalar;

			vector->scalar = geometricProduct;
		}

		*resultOperand = vector;
		return true;
	}

	if( leftVector && rightOperand->IsScalar() )
	{
		Vector* vector = ( Vector* )leftVector->Clone();

		if( !vector->scalar )
			vector->scalar = rightOperand->Clone();
		else
		{
			GeometricProduct* geometricProduct = new GeometricProduct();
			geometricProduct->operandList.InsertAfter()->data = vector->scalar;
			geometricProduct->operandList.InsertAfter()->data = rightOperand->Clone();

			vector->scalar = geometricProduct;
		}

		*resultOperand = vector;
		return true;
	}

	const Product* leftProduct = dynamic_cast< const Product* >( leftOperand );
	const Product* rightProduct = dynamic_cast< const Product* >( rightOperand );

	if( leftOperand->IsScalar() && rightProduct && rightProduct->FindLeadingScaledSymbol() )
	{
		Product* product = ( Product* )rightProduct->Clone();
		ScaledSymbol* scaledSymbol = product->FindLeadingScaledSymbol();

		if( !scaledSymbol->scalar )
			scaledSymbol->scalar = leftOperand->Clone();
		else
		{
			GeometricProduct* geometricProduct = new GeometricProduct();
			geometricProduct->operandList.InsertAfter()->data = scaledSymbol->scalar;
			geometricProduct->operandList.InsertAfter()->data = leftOperand->Clone();

			scaledSymbol->scalar = geometricProduct;
		}

		*resultOperand = product;
		return true;
	}

	if( leftProduct && leftProduct->FindLeadingScaledSymbol() && rightOperand->IsScalar() )
	{
		Product* product = ( Product* )leftProduct->Clone();
		ScaledSymbol* scaledSymbol = product->FindLeadingScaledSymbol();

		if( !scaledSymbol->scalar )
			scaledSymbol->scalar = rightOperand->Clone();
		else
		{
			GeometricProduct* geometricProduct = new GeometricProduct();
			geometricProduct->operandList.InsertAfter()->data = scaledSymbol->scalar;
			geometricProduct->operandList.InsertAfter()->data = rightOperand->Clone();

			scaledSymbol->scalar = geometricProduct;
		}

		*resultOperand = product;
		return true;
	}

	return false;
}

int Product::CountScaledSymbols( void ) const
{
	int count = 0;

	for( const OperandList::Node* node = operandList.Head(); node; node = node->Next() )
	{
		const ScaledSymbol* scaledSymbol = dynamic_cast< const ScaledSymbol* >( node->data );
		if( scaledSymbol && scaledSymbol->scalar )
			count++;

		const Product* product = dynamic_cast< Product* >( node->data );
		if( product )
			count += product->CountScaledSymbols();
	}

	return count;
}

const ScaledSymbol* Product::FindLeadingScaledSymbol( void ) const
{
	return const_cast< Product* >( this )->FindLeadingScaledSymbol();
}

ScaledSymbol* Product::FindLeadingScaledSymbol( void )
{
	for( OperandList::Node* node = operandList.Head(); node; node = node->Next() )
	{
		ScaledSymbol* scaledSymbol = dynamic_cast< ScaledSymbol* >( node->data );
		if( scaledSymbol )
			return scaledSymbol;

		Product* product = dynamic_cast< Product* >( node->data );
		if( product )
		{
			scaledSymbol = product->FindLeadingScaledSymbol();
			if( scaledSymbol )
				return scaledSymbol;
		}
	}

	return nullptr;
}

void Product::StripScalars( OperandList& scalarList )
{
	for( OperandList::Node* node = operandList.Head(); node; node = node->Next() )
	{
		ScaledSymbol* scaledSymbol = dynamic_cast< ScaledSymbol* >( node->data );
		if( scaledSymbol && scaledSymbol->scalar )
		{
			scalarList.InsertAfter()->data = scaledSymbol->scalar;
			scaledSymbol->scalar = nullptr;
		}

		Product* product = dynamic_cast< Product* >( node->data );
		if( product )
			product->StripScalars( scalarList );
	}
}

void Product::CloneScalars( OperandList& scalarList ) const
{
	for( const OperandList::Node* node = operandList.Head(); node; node = node->Next() )
	{
		const ScaledSymbol* scaledSymbol = dynamic_cast< ScaledSymbol* >( node->data );
		if( scaledSymbol && scaledSymbol->scalar )
			scalarList.InsertAfter()->data = scaledSymbol->scalar->Clone();

		Product* product = dynamic_cast< Product* >( node->data );
		if( product )
			product->CloneScalars( scalarList );
	}
}

bool Product::ConsolidateScalars( void )
{
	int count = CountScaledSymbols();
	if( count >= 1 )
	{
		ScaledSymbol* leadingScaledSymbol = FindLeadingScaledSymbol();
		if( !leadingScaledSymbol->scalar || count > 1 )
		{
			OperandList scalarList;
			StripScalars( scalarList );

			Operand* scalar = nullptr;

			if( scalarList.Count() == 1 )
			{
				OperandList::Node* node = scalarList.Head();
				scalar = node->data;
				node->data = nullptr;
			}
			else
			{
				GeometricProduct* geometricProduct = new GeometricProduct();
				geometricProduct->operandList.AbsorbAfter( nullptr, &scalarList );
				scalar = geometricProduct;
			}

			leadingScaledSymbol->scalar = scalar;
			return true;
		}
	}

	return false;
}

bool Product::HasAScalar( void ) const
{
	for( const OperandList::Node* node = operandList.Head(); node; node = node->Next() )
		if( node->data->IsScalar() )
			return true;

	return false;
}

bool Product::IsHomogeneousOfScalars( void ) const
{
	for( const OperandList::Node* node = operandList.Head(); node; node = node->Next() )
		if( !node->data->IsScalar() )
			return false;

	return true;
}

bool Product::IsHomogeneousOfVectors( void ) const
{
	for( const OperandList::Node* node = operandList.Head(); node; node = node->Next() )
		if( !dynamic_cast< const Vector* >( node->data ) )
			return false;

	return true;
}

bool Product::IsHomogeneousOfScaledSymbols( void ) const
{
	for( const OperandList::Node* node = operandList.Head(); node; node = node->Next() )
		if( !dynamic_cast< const ScaledSymbol* >( node->data ) )
			return false;

	return true;
}

bool Product::IsHomogeneousOfSymbolicReals( void ) const
{
	for( const OperandList::Node* node = operandList.Head(); node; node = node->Next() )
		if( !dynamic_cast< const SymbolicReal* >( node->data ) )
			return false;

	return true;
}

bool Product::IsHomogeneousOfBlades( int minimumGrade ) const
{
	for( const OperandList::Node* node = operandList.Head(); node; node = node->Next() )
	{
		const OuterProduct* outerProduct = dynamic_cast< const OuterProduct* >( node->data );
		if( !outerProduct )
			return false;
		if( outerProduct->operandList.Count() < minimumGrade )
			return false;
		if( !outerProduct->IsHomogeneousOfVectors() )
			return false;
	}

	return true;
}

bool Product::IsHomogeneousOfVersors( int minimumGrade ) const
{
	for( const OperandList::Node* node = operandList.Head(); node; node = node->Next() )
	{
		const GeometricProduct* geometricProduct = dynamic_cast< const GeometricProduct* >( node->data );
		if( !geometricProduct )
			return false;
		if( geometricProduct->operandList.Count() < minimumGrade )
			return false;
		if( !geometricProduct->IsHomogeneousOfVectors() )	// We actually don't care if the vectors are each invertible, so the term "versor" is really a misnomer.
			return false;
	}

	return true;
}

// Product.cpp