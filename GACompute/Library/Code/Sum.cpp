// Sum.cpp

#include "GACompute.h"

using namespace GACompute;

Sum::Sum( void )
{
}

/*virtual*/ Sum::~Sum( void )
{
}

/*virtual*/ Operand* Sum::Create( void ) const
{
	return new Sum();
}

/*virtual*/ Operand* Sum::CreateVacuousCase( void ) const
{
	NumericScalar* numericScalar = new NumericScalar();
	numericScalar->SetReal( 0.0 );
	return numericScalar;
}

/*virtual*/ bool Sum::TakesPrecedenceOver( const Operation* operation ) const
{
	const Sum* sum = dynamic_cast< const Sum* >( operation );
	return( sum ? true : false );
}

/*virtual*/ bool Sum::CanAssociateWith( const Operation* operation ) const
{
	const Sum* sum = dynamic_cast< const Sum* >( operation );
	return( sum ? true : false );
}

/*virtual*/ const char* Sum::OperatorSymbol( PrintFormat printFormat ) const
{
	switch( printFormat )
	{
		case PRINT_NORMAL:
		{
			return " + ";
		}
		case PRINT_LATEX:
		{
			return "+";
		}
	}

	return "?";
}

// When empty, we are considered to be the additive identity.
/*virtual*/ void Sum::PrintWhenEmpty( char* buffer, int bufferSize, PrintFormat printFormat ) const
{
	buffer[0] = '\0';

	switch( printFormat )
	{
		case PRINT_NORMAL:
		{
			strcpy_s( buffer, bufferSize, "(S-Empty)" );
			break;
		}
		case PRINT_LATEX:
		{
			strcpy_s( buffer, bufferSize, "\\left(\\mbox{S-Empty}\\right)" );
			break;
		}
	}
}

/*virtual*/ Operand* Sum::Simplify( const SimplificationContext& simplificationContext )
{
	if( Associate() )
		return this;

	Operand* operand = MakeNonDegenerate();
	if( operand )
		return operand;

	if( CullZeroOperands() )
		return this;

	operand = Operation::Simplify( simplificationContext );
	if( operand )
		return operand;

	if( Sort() > 0 )
		return this;

	if( CombineLikeOperands() )
		return this;

	return 0;
}

bool Sum::CombineLikeOperands( void )
{
	for( OperandList::Node* nodeA = operandList->Head(); nodeA; nodeA = nodeA->Next() )
	{
		Operand* operandA = nodeA->data;

		for( OperandList::Node* nodeB = nodeA->Next(); nodeB; nodeB = nodeB->Next() )
		{
			Operand* operandB = nodeB->data;

			if( operandA->IsScalar() && operandB->IsScalar() )
			{
				Operand* scalar = AddScalars( operandA, operandB );
				if( scalar )
				{
					operandList->InsertAfter()->data = scalar;
					operandList->Remove( nodeA );
					operandList->Remove( nodeB );
					return true;
				}
			}
			else if( dynamic_cast< Vector* >( operandA ) && dynamic_cast< Vector* >( operandB ) )
			{
				Vector* vectorA = ( Vector* )operandA;
				Vector* vectorB = ( Vector* )operandB;

				if( 0 == strcmp( vectorA->GetName(), vectorB->GetName() ) )
				{
					Operand* scalarA = vectorA->GetScalar();
					Operand* scalarB = vectorB->GetScalar();

					vectorA->SetScalar( 0, false );
					vectorB->SetScalar( 0, false );

					if( !scalarA )
					{
						NumericScalar* numericScalar = new NumericScalar();
						numericScalar->SetReal( 1.0 );
						scalarA = numericScalar;
					}

					if( !scalarB )
					{
						NumericScalar* numericScalar = new NumericScalar();
						numericScalar->SetReal( 1.0 );
						scalarB = numericScalar;
					}

					Sum* scalar = new Sum();
					scalar->operandList->InsertAfter()->data = scalarA;
					scalar->operandList->InsertAfter()->data = scalarB;

					vectorB->SetScalar( scalar );
					operandList->Remove( nodeA );

					return true;
				}
			}
			else if( ( operandA->IsBlade() && operandB->IsBlade() ) ||
					( operandA->IsVersor() && operandB->IsVersor() ) )
			{
				Operation* operationA = ( Operation* )operandA;
				Operation* operationB = ( Operation* )operandB;

				if( OperationsAlike( operationA, operationB ) )
				{
					// Degenerates are removed well before we can get here, so this shouldn't happen.
					Vector* vector = operationB->FindLeadingVector();
					if( !vector )
					{
						Error* error = new Error();
						error->Format( "Sum expected to find a leading vector, but didn't." );
						throw error;
					}

					// An empty geometric product in either case will do fine.
					GeometricProduct* scalarA = operationA->StripScalars();
					GeometricProduct* scalarB = operationB->StripScalars();

					Sum* scalar = new Sum();
					scalar->operandList->InsertAfter()->data = scalarA;
					scalar->operandList->InsertAfter()->data = scalarB;

					vector->MarryWithScalar( scalar );
					operandList->Remove( nodeA );

					return true;
				}
			}
		}
	}

	return false;
}

Operand* Sum::AddScalars( const Operand* operandA, const Operand* operandB )
{
	List< const Operand* > operandListA, operandListB;
	GeometricProduct* geometricProduct = 0;

	try
	{
		if( dynamic_cast< const NumericScalar* >( operandA ) ||
			dynamic_cast< const SymbolicScalar* >( operandA ) ||
			dynamic_cast< const InnerProductScalar* >( operandA ) )
		{
			operandListA.InsertAfter()->data = operandA;
		}
		else if( dynamic_cast< const GeometricProduct* >( operandA ) )
		{
			const GeometricProduct* geometricProductA = ( const GeometricProduct* )operandA;
			for( OperandList::Node* nodeA = geometricProductA->operandList->Head(); nodeA; nodeA = nodeA->Next() )
				operandListA.InsertAfter()->data = nodeA->data;
		}
		else
			throw false;

		if( dynamic_cast< const NumericScalar* >( operandB ) ||
			dynamic_cast< const SymbolicScalar* >( operandB ) ||
			dynamic_cast< const InnerProductScalar* >( operandB ) )
		{
			operandListB.InsertAfter()->data = operandB;
		}
		else if( dynamic_cast< const GeometricProduct* >( operandB ) )
		{
			const GeometricProduct* geometricProductB = ( const GeometricProduct* )operandB;
			for( OperandList::Node* nodeB = geometricProductB->operandList->Head(); nodeB; nodeB = nodeB->Next() )
				operandListB.InsertAfter()->data = nodeB->data;
		}
		else
			throw false;

		const NumericScalar* numericScalarA = 0;
		List< const Operand* >::Node* nodeA = operandListA.Head();
		while( nodeA )
		{
			if( dynamic_cast< const NumericScalar* >( nodeA->data ) )
			{
				numericScalarA = ( const NumericScalar* )nodeA->data;
				nodeA->data = 0;
				operandListA.Remove( nodeA );
				break;
			}

			nodeA = nodeA->Next();
		}

		const NumericScalar* numericScalarB = 0;
		List< const Operand* >::Node* nodeB = operandListB.Head();
		while( nodeB )
		{
			if( dynamic_cast< const NumericScalar* >( nodeB->data ) )
			{
				numericScalarB = ( const NumericScalar* )nodeB->data;
				nodeB->data = 0;
				operandListB.Remove( nodeB );
				break;
			}

			nodeB = nodeB->Next();
		}

		if( operandListA.Count() != operandListB.Count() )
			throw false;

		// Note that the following algorithm assumes that all geometric products of scalars
		// have been sorted and collected by this point.  Otherwise, it won't work.
		nodeA = operandListA.Head();
		nodeB = operandListB.Head();
		while( nodeA && nodeB )
		{
			bool bail = true;

			if( dynamic_cast< const SymbolicScalar* >( nodeA->data ) && dynamic_cast< const SymbolicScalar* >( nodeB->data ) )
			{
				const SymbolicScalar* symbolicScalarA = ( const SymbolicScalar* )nodeA->data;
				const SymbolicScalar* symbolicScalarB = ( const SymbolicScalar* )nodeB->data;

				if( symbolicScalarA->GetExponent() == symbolicScalarB->GetExponent() &&
					0 == strcmp( symbolicScalarA->GetName(), symbolicScalarB->GetName() ) )
				{
					bail = false;
				}
			}
			else if( dynamic_cast< const InnerProductScalar* >( nodeA->data ) && dynamic_cast< const InnerProductScalar* >( nodeB->data ) )
			{
				const InnerProductScalar* innerProductScalarA = ( const InnerProductScalar* )nodeA->data;
				const InnerProductScalar* innerProductScalarB = ( const InnerProductScalar* )nodeB->data;

				if( innerProductScalarA->GetExponent() == innerProductScalarB->GetExponent() )
				{
					if( ( ( 0 == strcmp( innerProductScalarA->GetNameA(), innerProductScalarB->GetNameA() ) ) &&
						( 0 == strcmp( innerProductScalarA->GetNameB(), innerProductScalarB->GetNameB() ) ) ) ||
						( ( 0 == strcmp( innerProductScalarA->GetNameA(), innerProductScalarB->GetNameB() ) ) &&
						( 0 == strcmp( innerProductScalarA->GetNameB(), innerProductScalarB->GetNameA() ) ) ) )
					{
						bail = false;
					}
				}
			}

			if( bail )
				throw false;

			nodeA = nodeA->Next();
			nodeB = nodeB->Next();
		}

		NumericScalar* numericScalar = new NumericScalar();
		if( numericScalarA && numericScalarB )
			numericScalar->SetReal( numericScalarA->GetReal() + numericScalarB->GetReal() );
		else if( numericScalarA )
			numericScalar->SetReal( numericScalarA->GetReal() + 1.0 );
		else if( numericScalarB )
			numericScalar->SetReal( 1.0 + numericScalarB->GetReal() );
		else
			numericScalar->SetReal( 2.0 );

		geometricProduct = new GeometricProduct();
		geometricProduct->operandList->InsertAfter()->data = numericScalar;

		for( nodeA = operandListA.Head(); nodeA; nodeA = nodeA->Next() )
			geometricProduct->operandList->InsertAfter()->data = nodeA->data->Clone();
	}
	catch( ... )
	{
	}

	operandListA.NullifyData();
	operandListB.NullifyData();

	return geometricProduct;
}

// We assume here that the given operations are indeed blades or versors.
// Also note that in the case of blades, this test doesn't work if the given
// blades were not pre-sorted.
bool Sum::OperationsAlike( const Operation* operationA, const Operation* operationB )
{
	if( operationA->operandList->Count() != operationB->operandList->Count() )
		return false;

	OperandList::Node* nodeA = operationA->operandList->Head();
	OperandList::Node* nodeB = operationB->operandList->Head();

	while( nodeA && nodeB )
	{
		Vector* vectorA = ( Vector* )nodeA->data;
		Vector* vectorB = ( Vector* )nodeB->data;

		if( 0 != strcmp( vectorA->GetName(), vectorB->GetName() ) )
			return false;

		nodeA = nodeA->Next();
		nodeB = nodeB->Next();
	}

	return true;
}

/*virtual*/ bool Sum::IsAdditiveIdentity( void ) const
{
	// Notice that this handles the vacuous case too.
	return AllOperandsAdditiveIdentity();
}

bool Sum::CullZeroOperands( void )
{
	bool culled = false;

	OperandList::Node* node = operandList->Head();
	while( node )
	{
		OperandList::Node* nextNode = node->Next();

		if( node->data->IsAdditiveIdentity() )
		{
			operandList->Remove( node );
			culled = true;
		}

		node = nextNode;
	}

	return culled;
}

// Sum.cpp