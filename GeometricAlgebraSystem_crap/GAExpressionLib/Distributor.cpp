// Distributor.cpp

#include "Distributor.h"
#include "Operation.h"

using namespace GAExpressionLib;

Distributor::Distributor( void )
{
}

/*virtual*/ Distributor::~Distributor( void )
{
}

/*virtual*/ bool Distributor::Manipulate( ManipulationData& data )
{
	bool distributed = false;

	Operation* operation = dynamic_cast< Operation* >( data.oldOperand );
	if( operation )
	{
		for( Operation::OperandList::Node* node = operation->operandList.Head(); node; node = node->Next() )
		{
			Operation* nestedOperation = dynamic_cast< Operation* >( node->data );
			if( nestedOperation && operation->DistributesOver( nestedOperation ) )
			{
				Operation::OperandList leftOperandList, rightOperandList;

				while( operation->operandList.Head() != node )
				{
					Operation::OperandList::Node* leftNode = operation->operandList.Head();
					operation->operandList.Remove( leftNode, false );
					leftOperandList.InsertAfter( leftOperandList.Tail(), leftNode );
				}

				while( operation->operandList.Tail() != node )
				{
					Operation::OperandList::Node* rightNode = operation->operandList.Tail();
					operation->operandList.Remove( rightNode, false );
					rightOperandList.InsertBefore( rightOperandList.Head(), rightNode );
				}

				for( Operation::OperandList::Node* nestedNode = nestedOperation->operandList.Head(); nestedNode; nestedNode = nestedNode->Next() )
				{
					Operation* newOperation = ( Operation* )operation->Allocate();
					Operation::OperandList::Node* newNode = nestedOperation->operandList.InsertBefore( nestedNode );
					newNode->data = newOperation;
					nestedOperation->operandList.Remove( nestedNode, false );

					newOperation->operandList.InsertAfter( nullptr, nestedNode );
					newOperation->operandList.CopyBefore( newOperation->operandList.Head(), &leftOperandList );
					newOperation->operandList.CopyAfter( newOperation->operandList.Tail(), &rightOperandList );

					nestedNode = newNode;
				}

				node->data = nullptr;
				data.newOperand = nestedOperation;

				distributed = true;

				break;
			}
		}
	}

	return distributed;
}

// Distributor.cpp