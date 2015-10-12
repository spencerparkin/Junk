// Operation.cpp

#include "GACompute.h"

using namespace GACompute;

Operation::Operation( void )
{
	operandList = new OperandList();
}

/*virtual*/ Operation::~Operation( void )
{
	delete operandList;
}

/*virtual*/ void Operation::Print( char* buffer, int bufferSize, PrintFormat printFormat ) const
{
	buffer[0] = '\0';

	if( operandList->Count() == 0 )
	{
		PrintWhenEmpty( buffer, bufferSize, printFormat );
		return;
	}

	for( OperandList::Node* node = operandList->Head(); node; node = node->Next() )
	{
		const Operand* operand = node->data;
		char subBuffer[ 2048 ];
		operand->Print( subBuffer, sizeof( subBuffer ), printFormat );

		bool parenthesisNeeded = false;
		const Operation* operation = dynamic_cast< const Operation* >( operand );
		if( operation && TakesPrecedenceOver( operation ) )
			parenthesisNeeded = true;

		if( parenthesisNeeded )
		{
			switch( printFormat )
			{
				case PRINT_NORMAL:
				{
					strcat_s( buffer, bufferSize, "(" );
					break;
				}
				case PRINT_LATEX:
				{
					strcat_s( buffer, bufferSize, "\\left(" );
					break;
				}
			}
		}
		
		strcat_s( buffer, bufferSize, subBuffer );

		if( parenthesisNeeded )
		{
			switch( printFormat )
			{
				case PRINT_NORMAL:
				{
					strcat_s( buffer, bufferSize, ")" );
					break;
				}
				case PRINT_LATEX:
				{
					strcat_s( buffer, bufferSize, "\\right)" );
					break;
				}
			}
		}

		if( node->Next() )
			strcat_s( buffer, bufferSize, OperatorSymbol( printFormat ) );
	}
}

/*virtual*/ Operand* Operation::Clone( void ) const
{
	Operation* operation = ( Operation* )Create();
	if( operation )
		for( OperandList::Node* node = operandList->Head(); node; node = node->Next() )
			operation->operandList->InsertAfter()->data = node->data->Clone();

	return operation;
}

/*virtual*/ Operand* Operation::Create( void ) const
{
	return 0;
}

/*virtual*/ Operand* Operation::CreateVacuousCase( void ) const
{
	return 0;
}

/*virtual*/ Operand* Operation::MakeNonDegenerate( void )
{
	if( operandList->Count() > 1 )
		return 0;

	if( operandList->Count() == 1 )
	{
		OperandList::Node* node = operandList->Head();
		Operand* operand = node->data;
		node->data = 0;
		operandList->Remove( node );
		return operand;
	}

	Operand* operand = CreateVacuousCase();
	return operand;
}

/*virtual*/ Operand* Operation::Simplify( const SimplificationContext& simplificationContext )
{
	SimplificationContext simplificationSubContext;
	simplificationSubContext.target = simplificationContext.target;
	simplificationSubContext.operation = this;

	// Note here that we could certainly get away with simplifying more than one
	// operand here, but by only allowing one to simplify per call, we can get a
	// net gain in performance.  The reason is because we're doing what we can here
	// to minimize the creation and distribution of new terms.  For example, consider
	// (a^b)(c^d).  If we allow both operands of the geometric product to simplify,
	// we may get (ab-a.b)(cd-c.d), which is considerably more work to expand than
	// the expression (ab-a.b)(c^d).
	for( OperandList::Node* node = operandList->Head(); node; node = node->Next() )
		if( Operand::Simplify( node->data, simplificationSubContext ) )
			return this;

	return 0;
}

Operand* Operation::Distribute( void )
{
	OperandList::Node* node = operandList->Head();
	while( node )
	{
		Operation* operation = dynamic_cast< Operation* >( node->data );
		if( operation && CanDistributeOver( operation ) )
			break;
		node = node->Next();
	}

	if( !node )
		return 0;

	Operation* operationA = ( Operation* )node->data;
	Operation* operationB = this;
	Operation* operationC = ( Operation* )operationA->Create();

	for( OperandList::Node* nodeA = operationA->operandList->Head(); nodeA; nodeA = nodeA->Next() )
	{
		Operation* operationD = ( Operation* )operationB->Create();

		OperandList::Node* nodeB = operationB->operandList->Head();
		while( nodeB != node )
		{
			operationD->operandList->InsertAfter()->data = nodeB->data->Clone();
			nodeB = nodeB->Next();
		}
		
		operationD->operandList->InsertAfter()->data = nodeA->data->Clone();

		nodeB = nodeB->Next();
		while( nodeB )
		{
			operationD->operandList->InsertAfter()->data = nodeB->data->Clone();
			nodeB = nodeB->Next();
		}

		operationC->operandList->InsertAfter()->data = operationD;
	}

	return operationC;
}

bool Operation::Associate( void )
{
	bool associated = false;

	for( OperandList::Node* node = operandList->Head(); node; node = node->Next() )
	{
		Operation* operation = dynamic_cast< Operation* >( node->data );
		if( !operation )
			continue;

		if( !CanAssociateWith( operation ) )
			continue;

		OperandList::Node* insertAfter = node;
		while( operation->operandList->Count() > 0 )
		{
			OperandList::Node* head = operation->operandList->Head();
			operation->operandList->Remove( head, false );
			operandList->InsertAfter( insertAfter, head );
			insertAfter = head;
		}

		operandList->Remove( node );
		node = insertAfter;
		associated = true;
	}

	return associated;
}

// Notice that we return an appropriate result in the vacuous case.
// This function seperates a blade or versor or inner-product from its scalar.
GeometricProduct* Operation::StripScalars( void )
{
	GeometricProduct* geometricProduct = new GeometricProduct();

	OperandList::Node* node = operandList->Head();
	while( node )
	{
		OperandList::Node* nextNode = node->Next();

		if( node->data->IsScalar() )
		{
			operandList->Remove( node, false );
			geometricProduct->operandList->InsertAfter( 0, node );
		}
		else if( dynamic_cast< Vector* >( node->data ) )
		{
			Vector* vector = ( Vector* )node->data;
			if( vector->GetScalar() )
			{
				geometricProduct->operandList->InsertAfter()->data = vector->GetScalar();
				vector->SetScalar( 0, false );
			}
		}

		node = nextNode;
	}

	return geometricProduct;
}

Vector* Operation::FindLeadingVector( void )
{
	Vector* vector = 0;
	for( OperandList::Node* node = operandList->Head(); node && !vector; node = node->Next() )
		vector = dynamic_cast< Vector* >( node->data );
	return vector;
}

// This assumes we're some sort of product, and notice that we're not recursive, though
// concievably, we could be, through operations that are also products.
bool Operation::MarryScalars( void )
{
	bool marriedScalar = false;

	Vector* vector = FindLeadingVector();
	if( vector )
	{
		int count = 0;
		for( OperandList::Node* node = operandList->Head(); node; node = node->Next() )
			if( node->data->IsScalar() || ( dynamic_cast< Vector* >( node->data ) && ( ( Vector* )node->data )->GetScalar() && node->data != vector ) )
				count++;

		if( count > 0 )
		{
			GeometricProduct* geometricProduct = StripScalars();
			vector->MarryWithScalar( geometricProduct );
			marriedScalar = true;
		}
	}

	return marriedScalar;
}

void Operation::CountOperands( OperandCounts& operandCounts ) const
{
	operandCounts.scalarCount = 0;
	operandCounts.versorCount = 0;
	operandCounts.bladeCount = 0;

	for( OperandList::Node* node = operandList->Head(); node; node = node->Next() )
	{
		const Operand* operand = node->data;

		if( operand->IsScalar() )
			operandCounts.scalarCount++;
		else if( operand->IsBlade() )
			operandCounts.bladeCount++;
		else if( operand->IsVersor() )
			operandCounts.versorCount++;
	}
}

/*virtual*/ bool Operation::IsScalar( void ) const
{
	for( OperandList::Node* node = operandList->Head(); node; node = node->Next() )
		if( !node->data->IsScalar() )
			return false;

	return true;
}

/*virtual*/ bool Operation::TakesPrecedenceOver( const Operation* operation ) const
{
	return false;
}

/*virtual*/ bool Operation::CanDistributeOver( const Operation* operation ) const
{
	return false;
}

/*virtual*/ bool Operation::CanAssociateWith( const Operation* operation ) const
{
	return false;
}

bool Operation::AllOperandsAdditiveIdentity( void ) const
{
	for( OperandList::Node* node = operandList->Head(); node; node = node->Next() )
		if( !node->data->IsAdditiveIdentity() )
			return false;
	return true;
}

bool Operation::AnyOperandAdditiveIdentity( void ) const
{
	for( OperandList::Node* node = operandList->Head(); node; node = node->Next() )
		if( node->data->IsAdditiveIdentity() )
			return true;
	return false;
}

/*virtual*/ void Operation::SortKey( char* buffer, int bufferSize ) const
{
	sprintf_s( buffer, bufferSize, "operation_%d", operandList->Count() );
}

/*static*/ int Operation::SortCompare( const Operand* operandA, const Operand* operandB, void* data )
{
	const Operation* operation = ( Operation* )data;
	return operation->SortCompareOperands( operandA, operandB );
}

/*virtual*/ int Operation::SortCompareOperands( const Operand* operandA, const Operand* operandB ) const
{
	char sortKeyA[ 512 ];
	char sortKeyB[ 512 ];

	operandA->SortKey( sortKeyA, sizeof( sortKeyA ) );
	operandB->SortKey( sortKeyB, sizeof( sortKeyB ) );

	return strcmp( sortKeyA, sortKeyB );
}

/*virtual*/ int Operation::Sort( void )
{
	int adjacentSwapCount = operandList->Sort( OperandList::SORT_ASCENDING, ( OperandList::SortCompareFunc )SortCompare, this );
	return adjacentSwapCount;
}

// Operation.cpp