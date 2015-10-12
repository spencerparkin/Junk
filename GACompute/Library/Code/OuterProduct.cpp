// OuterProduct.cpp

#include "GACompute.h"

using namespace GACompute;

OuterProduct::OuterProduct( void )
{
}

/*virtual*/ OuterProduct::~OuterProduct( void )
{
}

/*virtual*/ Operand* OuterProduct::Create( void ) const
{
	return new OuterProduct();
}

/*virtual*/ Operand* OuterProduct::CreateVacuousCase( void ) const
{
	NumericScalar* numericScalar = new NumericScalar();
	numericScalar->SetReal( 1.0 );
	return numericScalar;
}

/*virtual*/ Operand* OuterProduct::Simplify( const SimplificationContext& simplificationContext )
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

	operand = Operation::Simplify( simplificationContext );
	if( operand )
		return operand;

	operand = MakeNonDegenerate();
	if( operand )
		return operand;
	
	if( MarryScalars() )
		return this;

	if( IsScalar() )
	{
		GeometricProduct* geometricProduct = new GeometricProduct();
		while( operandList->Count() > 0 )
		{
			OperandList::Node* node = operandList->Head();
			operandList->Remove( node, false );
			geometricProduct->operandList->InsertAfter( 0, node );
		}
		return geometricProduct;
	}

	if( !IsBlade() )
	{
		Error* error = new Error();
		error->Format( "Outer product expected a blade at this point of simplification." );
		throw error;
	}

	if( Sort() > 0 )
		return this;

	if( ShouldExpand( simplificationContext ) )
	{
		if( operandList->Count() < 2 )
		{
			Error* error = new Error();
			error->Format( "Outer product expected a non-degenerate case here." );
			throw error;
		}

		NumericScalar* numericScalar = new NumericScalar();
		numericScalar->SetReal( -1.0 );

		OperandList::Node* node = operandList->Head();
		operandList->Remove( node, false );
		
		GeometricProduct* geometricProduct = new GeometricProduct();
		geometricProduct->operandList->InsertAfter( 0, node );
		geometricProduct->operandList->InsertAfter()->data = Clone();

		InnerProduct* innerProduct = new InnerProduct();
		innerProduct->operandList->InsertAfter()->data = numericScalar;
		innerProduct->operandList->InsertAfter()->data = node->data->Clone();
		innerProduct->operandList->InsertAfter()->data = Clone();

		Sum* sum = new Sum();
		sum->operandList->InsertAfter()->data = geometricProduct;
		sum->operandList->InsertAfter()->data = innerProduct;
		return sum;
	}

	return 0;
}

bool OuterProduct::ShouldExpand( const SimplificationContext& simplificationContext ) const
{
	switch( simplificationContext.target )
	{
		case SimplificationContext::SUM_OF_VERSORS:
		{
			if( dynamic_cast< const InnerProduct* >( simplificationContext.operation ) )
				return false;
			return true;
		}
		case SimplificationContext::SUM_OF_BLADES:
		{
			const GeometricProduct* geometricProduct = dynamic_cast< const GeometricProduct* >( simplificationContext.operation );
			if( geometricProduct )
			{
				for( OperandList::Node* node = geometricProduct->operandList->Head(); node; node = node->Next() )
				{
					const Operand* operand = node->data;
					if( operand == this )
						continue;

					if( !operand->IsScalar() )
						return true;
				}
			}

			return false;
		}
	}

	return false;
}

/*virtual*/ int OuterProduct::Sort( void )
{
	int adjacentSwapCount = Operation::Sort();

	// Note that this count would be wrong if, as a blade, we had not already married off all scalars.
	if( adjacentSwapCount % 2 == 1 )
	{
		NumericScalar* numericScalar = new NumericScalar();
		numericScalar->SetReal( -1.0 );

		GeometricProduct* geometricProduct = StripScalars();
		geometricProduct->operandList->InsertBefore()->data = numericScalar;

		Vector* vector = FindLeadingVector();
		vector->MarryWithScalar( geometricProduct );
	}

	return adjacentSwapCount;
}

/*virtual*/ bool OuterProduct::TakesPrecedenceOver( const Operation* operation ) const
{
	if( dynamic_cast< const Sum* >( operation ) ||
		dynamic_cast< const InnerProduct* >( operation ) ||
		dynamic_cast< const OuterProduct* >( operation ) )
	{
		return true;
	}

	return true;
}

/*virtual*/ bool OuterProduct::CanDistributeOver( const Operation* operation ) const
{
	const Sum* sum = dynamic_cast< const Sum* >( operation );
	return( sum ? true : false );
}

/*virtual*/ bool OuterProduct::CanAssociateWith( const Operation* operation ) const
{
	const OuterProduct* outerProduct = dynamic_cast< const OuterProduct* >( operation );
	return( outerProduct ? true : false );
}

/*virtual*/ const char* OuterProduct::OperatorSymbol( PrintFormat printFormat ) const
{
	switch( printFormat )
	{
		case PRINT_NORMAL:
		{
			return "^";
		}
		case PRINT_LATEX:
		{
			return "\\wedge ";
		}
	}

	return "?";
}

/*virtual*/ void OuterProduct::PrintWhenEmpty( char* buffer, int bufferSize, PrintFormat printFormat ) const
{
	buffer[0] = '\0';

	switch( printFormat )
	{
		case PRINT_NORMAL:
		{
			strcpy_s( buffer, bufferSize, "(OP-Empty)" );
			break;
		}
		case PRINT_LATEX:
		{
			strcpy_s( buffer, bufferSize, "\\left(\\mbox{OP-Empty}\\right)" );
			break;
		}
	}
}

/*virtual*/ bool OuterProduct::IsBlade( void ) const
{
	for( OperandList::Node* node = operandList->Head(); node; node = node->Next() )
		if( !dynamic_cast< const Vector* >( node->data ) )
			return false;		// We may still boil down to a blade, but for all we know, we're not a blade.

	return true;
}

/*virtual*/ bool OuterProduct::IsAdditiveIdentity( void ) const
{
	if( AnyOperandAdditiveIdentity() )
		return true;

	// Check for linear dependence by name.
	for( OperandList::Node* nodeA = operandList->Head(); nodeA; nodeA = nodeA->Next() )
	{
		const Vector* vectorA = dynamic_cast< const Vector* >( nodeA->data );
		if( !vectorA )
			continue;

		for( OperandList::Node* nodeB = nodeA->Next(); nodeB; nodeB = nodeB->Next() )
		{
			const Vector* vectorB = dynamic_cast< const Vector* >( nodeB->data );
			if( !vectorB )
				continue;

			if( 0 == strcmp( vectorA->GetName(), vectorB->GetName() ) )
				return true;
		}
	}

	// For all we know, we're non-zero.
	return false;
}

// OuterProduct.cpp