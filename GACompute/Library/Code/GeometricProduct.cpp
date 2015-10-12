// GeometricProduct.cpp

#include "GACompute.h"

using namespace GACompute;

GeometricProduct::GeometricProduct( void )
{
}

/*virtual*/ GeometricProduct::~GeometricProduct( void )
{
}

/*virtual*/ Operand* GeometricProduct::Create( void ) const
{
	return new GeometricProduct();
}

/*virtual*/ Operand* GeometricProduct::CreateVacuousCase( void ) const
{
	NumericScalar* numericScalar = new NumericScalar();
	numericScalar->SetReal( 1.0 );
	return numericScalar;
}

/*virtual*/ bool GeometricProduct::TakesPrecedenceOver( const Operation* operation ) const
{
	return true;
}

/*virtual*/ bool GeometricProduct::CanDistributeOver( const Operation* operation ) const
{
	if( dynamic_cast< const Sum* >( operation ) )
		return true;
	return false;
}

/*virtual*/ bool GeometricProduct::CanAssociateWith( const Operation* operation ) const
{
	if( dynamic_cast< const GeometricProduct* >( operation ) )
		return true;
	return false;
}

/*virtual*/ const char* GeometricProduct::OperatorSymbol( PrintFormat printFormat ) const
{
	switch( printFormat )
	{
		case PRINT_NORMAL:
		{
			return "*";
		}
		case PRINT_LATEX:
		{
			return "";	// Juxtaposition.
		}
	}

	return "?";
}

// When empty, we are the multiplicative identity with respect to the geometric product.
// Our print here makes it obvious what's going on in the tree, but is also something that,
// while not algebraically unsound, something that we'll want to avoid in any result.
/*virtual*/ void GeometricProduct::PrintWhenEmpty( char* buffer, int bufferSize, PrintFormat printFormat ) const
{
	buffer[0] = '\0';

	switch( printFormat )
	{
		case PRINT_NORMAL:
		{
			strcpy_s( buffer, bufferSize, "(GP-Empty)" );
			break;
		}
		case PRINT_LATEX:
		{
			strcpy_s( buffer, bufferSize, "\\left(\\mbox{GP-Empty}\\right)" );
			break;
		}
	}
}

// Strictly speaking, each vector must be invertible, and even if some operands
// are not vectors directly, they may evaluate as such indirectly, but here we
// are looking for a specific pattern, and we don't care about invertibility.
/*virtual*/ bool GeometricProduct::IsVersor( void ) const
{
	for( OperandList::Node* node = operandList->Head(); node; node = node->Next() )
		if( !dynamic_cast< const Vector* >( node->data ) )
			return false;

	return true;
}

/*virtual*/ Operand* GeometricProduct::Simplify( const SimplificationContext& simplificationContext )
{
	if( IsAdditiveIdentity() )
	{
		NumericScalar* numericScalar = new NumericScalar();
		numericScalar->SetReal( 0.0 );
		return numericScalar;
	}

	Operand* operand = Distribute();
	if( operand )
		return operand;

	if( Associate() )
		return this;

	operand = MakeNonDegenerate();
	if( operand )
		return operand;

	// This block of code is actually optional in that it shouldn't be necessary
	// for the correctness of our algorithm, but it may be a significant optimization.
	if( simplificationContext.target == SimplificationContext::SUM_OF_BLADES )
	{
		OperandList::Node* node = operandList->Head();
		while( node && node->Next() )
		{
			OperandList::Node* leftNode = node;
			OperandList::Node* rightNode = node->Next();

			Operand* leftOperand = leftNode->data;
			Operand* rightOperand = rightNode->data;

			if( dynamic_cast< Vector* >( leftOperand ) && rightOperand->IsBlade() &&
				( ( Operation* )rightOperand )->operandList->Count() > 1 ||
				leftOperand->IsBlade() && dynamic_cast< Vector* >( rightOperand ) &&
				( ( Operation* )leftOperand )->operandList->Count() > 1 )
			{
				InnerProduct* innerProduct = new InnerProduct();
				innerProduct->operandList->InsertAfter()->data = leftOperand->Clone();
				innerProduct->operandList->InsertAfter()->data = rightOperand->Clone();

				OuterProduct* outerProduct = new OuterProduct();
				outerProduct->operandList->InsertAfter()->data = leftOperand->Clone();
				outerProduct->operandList->InsertAfter()->data = rightOperand->Clone();

				Sum* sum = new Sum;
				sum->operandList->InsertAfter()->data = innerProduct;
				sum->operandList->InsertAfter()->data = outerProduct;
				
				operandList->InsertAfter( leftNode )->data = sum;
				operandList->Remove( leftNode );
				operandList->Remove( rightNode );

				return this;
			}

			node = node->Next();
		}
	}

	operand = Operation::Simplify( simplificationContext );
	if( operand )
		return operand;
	
	if( !IsScalar() )
	{
		OperandCounts operandCounts;
		CountOperands( operandCounts );
		if( operandCounts.bladeCount == 1 && operandCounts.versorCount == 0 && operandCounts.scalarCount > 0 )
		{
			OuterProduct* outerProduct = new OuterProduct();
			while( operandList->Count() > 0 )
			{
				OperandList::Node* node = operandList->Head();
				operandList->Remove( node, false );
				outerProduct->operandList->InsertAfter( 0, node );
			}
			return outerProduct;
		}

		if( MarryScalars() )
			return this;

		if( !IsVersor() )
		{
			Error* error = new Error();
			error->Format( "Geometric product expected a versor at this point of simplification." );
			throw error;
		}

		if( ShouldExpand( simplificationContext ) )
		{
			if( operandList->Count() < 2 )
			{
				Error* error = new Error();
				error->Format( "Geometric product expected a non-degenerate case here." );
				throw error;
			}

			// If V is an odd versor, and v is a vector, then vV = v.V = v^V.
			// However, if V is an even versor, then vV != v.V + v^V.
			OperandList::Node* nodeA = 0;
			if( operandList->Count() % 2 == 1 )
			{
				nodeA = operandList->Head();
				operandList->Remove( nodeA, false );
			}

			OperandList::Node* nodeB = operandList->Head();
			operandList->Remove( nodeB, false );

			InnerProduct* innerProduct = new InnerProduct();
			innerProduct->operandList->InsertAfter( 0, nodeB );
			innerProduct->operandList->InsertAfter()->data = Clone();

			OuterProduct* outerProduct = new OuterProduct();
			outerProduct->operandList->InsertAfter()->data = nodeB->data->Clone();
			outerProduct->operandList->InsertAfter()->data = Clone();

			Sum* sum = new Sum();
			sum->operandList->InsertAfter()->data = innerProduct;
			sum->operandList->InsertAfter()->data = outerProduct;

			if( !nodeA )
				return sum;
			
			GeometricProduct* geometricProduct = new GeometricProduct();
			geometricProduct->operandList->InsertAfter( 0, nodeA );
			geometricProduct->operandList->InsertAfter()->data = sum;
			return geometricProduct;
		}
	}
	else
	{
		if( CombineOperands() )
			return this;

		if( CullOneOperands() )
			return this;

		if( Sort() > 0 )
			return this;
	}

	return 0;
}

bool GeometricProduct::ShouldExpand( const SimplificationContext& simplificationContext ) const
{
	switch( simplificationContext.target )
	{
		case SimplificationContext::SUM_OF_VERSORS:
		{
			if( dynamic_cast< const OuterProduct* >( simplificationContext.operation ) ||
				dynamic_cast< const InnerProduct* >( simplificationContext.operation ) )
			{
				return true;
			}
			return false;
		}
		case SimplificationContext::SUM_OF_BLADES:
		{
			return true;
		}
	}

	return true;
}

bool GeometricProduct::CullOneOperands( void )
{
	int count = 0;

	OperandList::Node* node = operandList->Head();
	while( node )
	{
		OperandList::Node* nextNode = node->Next();

		if( node->data->IsMultiplicativeIdentity() )
		{
			operandList->Remove( node );
			count++;
		}

		node = nextNode;
	}

	return( count > 0 ? true : false );
}

bool GeometricProduct::CombineOperands( void )
{
	for( OperandList::Node* nodeA = operandList->Head(); nodeA; nodeA = nodeA->Next() )
	{
		Operand* operandA = nodeA->data;
		if( !operandA->IsScalar() )
			continue;

		for( OperandList::Node* nodeB = nodeA->Next(); nodeB; nodeB = nodeB->Next() )
		{
			Operand* operandB = nodeB->data;
			if( !operandB->IsScalar() )
				continue;

			Operand* scalar = MultiplyScalars( operandA, operandB );
			if( scalar )
			{
				operandList->InsertAfter()->data = scalar;
				operandList->Remove( nodeA );
				operandList->Remove( nodeB );
				return true;
			}
		}
	}

	return false;
}

Operand* GeometricProduct::MultiplyScalars( const Operand* operandA, const Operand* operandB )
{
	if( dynamic_cast< const NumericScalar* >( operandA ) && dynamic_cast< const NumericScalar* >( operandB ) )
	{
		const NumericScalar* numericScalarA = ( const NumericScalar* )operandA;
		const NumericScalar* numericScalarB = ( const NumericScalar* )operandB;
		
		NumericScalar* numericScalar = new NumericScalar();
		numericScalar->SetReal( numericScalarA->GetReal() * numericScalarB->GetReal() );
		return numericScalar;
	}
	else if( dynamic_cast< const SymbolicScalar* >( operandA ) && dynamic_cast< const SymbolicScalar* >( operandB ) )
	{
		const SymbolicScalar* symbolicScalarA = ( const SymbolicScalar* )operandA;
		const SymbolicScalar* symbolicScalarB = ( const SymbolicScalar* )operandB;

		if( 0 == strcmp( symbolicScalarA->GetName(), symbolicScalarB->GetName() ) )
		{
			SymbolicScalar* symbolicScalar = new SymbolicScalar();
			symbolicScalar->SetName( symbolicScalarA->GetName() );
			symbolicScalar->SetExponent( symbolicScalarA->GetExponent() + symbolicScalarB->GetExponent() );
			return symbolicScalar;
		}
	}
	else if( dynamic_cast< const InnerProductScalar* >( operandA ) && dynamic_cast< const InnerProductScalar* >( operandB ) )
	{
		const InnerProductScalar* innerProductScalarA = ( const InnerProductScalar* )operandA;
		const InnerProductScalar* innerProductScalarB = ( const InnerProductScalar* )operandB;

		if( ( ( 0 == strcmp( innerProductScalarA->GetNameA(), innerProductScalarB->GetNameA() ) ) &&
			( 0 == strcmp( innerProductScalarA->GetNameB(), innerProductScalarB->GetNameB() ) ) ) ||
			( ( 0 == strcmp( innerProductScalarA->GetNameA(), innerProductScalarB->GetNameB() ) ) &&
			( 0 == strcmp( innerProductScalarA->GetNameB(), innerProductScalarB->GetNameA() ) ) ) )
		{
			InnerProductScalar* innerProductScalar = new InnerProductScalar();
			innerProductScalar->SetNames( innerProductScalarA->GetNameA(), innerProductScalarA->GetNameB() );
			innerProductScalar->SetExponent( innerProductScalarA->GetExponent() + innerProductScalarB->GetExponent() );
			return innerProductScalar;
		}
	}

	return 0;
}

/*virtual*/ bool GeometricProduct::IsAdditiveIdentity( void ) const
{
	return AnyOperandAdditiveIdentity();
}

// In the vacuous case, we're the identity, but we delete those anyway.
/*virtual*/ bool GeometricProduct::IsMultiplicativeIdentity( void ) const
{
	if( operandList->Count() == 0 )
		return true;

	// We may still be the identity, but for all we know, we're not.
	return false;
}

// GeometricProduct.cpp