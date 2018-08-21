// Associator.cpp

#include "Associator.h"
#include "Operation.h"

using namespace GAExpressionLib;

Associator::Associator( void )
{
}

/*virtual*/ Associator::~Associator( void )
{
}

/*virtual*/ bool Associator::Manipulate( ManipulationData& data )
{
	bool associated = false;

	Operation* operation = dynamic_cast< Operation* >( data.oldOperand );
	if( operation )
	{
		Operation::OperandList::Node* node = operation->operandList.Head();
		while( node )
		{
			Operation::OperandList::Node* nextNode = node->Next();

			Operation* nestedOperation = dynamic_cast< Operation* >( node->data );
			if( nestedOperation && operation->AssociatesWith( nestedOperation ) )
			{
				operation->operandList.AbsorbAfter( node, &nestedOperation->operandList );
				operation->operandList.Remove( node );

				associated = true;
			}

			node = nextNode;
		}
	}

	return associated;
}

// Associator.cpp