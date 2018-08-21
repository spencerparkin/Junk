// Manipulator.cpp

#include "Manipulator.h"
#include "Operation.h"
#include "Vector.h"
#include "SymbolicReal.h"
#include "ScaledSymbol.h"

using namespace GAExpressionLib;

Manipulator::Manipulator( void )
{
}

/*virtual*/ Manipulator::~Manipulator( void )
{
}

/*virtual*/ bool Manipulator::ManipulateTree( Operand** operand )
{
	return ManipulateRecursively( operand );
}

/*virtual*/ bool Manipulator::Manipulate( ManipulationData& data )
{
	return false;
}

bool Manipulator::ManipulateRecursively( Operand** operand )
{
	bool manipulatedTree = false;

	while( true )
	{
		Operation* operation = dynamic_cast< Operation* >( *operand );
		if( operation )
		{
			Operation::OperandList::Node* node = operation->operandList.Head();
			while( node )
			{
				if( ManipulateRecursively( &node->data ) )
					manipulatedTree = true;

				node = node->Next();
			}
		}

		ScaledSymbol* scaledSymbol = dynamic_cast< ScaledSymbol* >( *operand );
		if( scaledSymbol && scaledSymbol->scalar )
		{
			if( ManipulateRecursively( &scaledSymbol->scalar ) )
				manipulatedTree = true;
		}

		ManipulationData data;
		data.oldOperand = *operand;
		data.newOperand = nullptr;

		// If the tree rooted here is manipulated, we descend down it too and continue
		// to do so with each new subtree until no more manipulations are made.
		if( !Manipulate( data ) )
			break;
		else
		{
			manipulatedTree = true;

			if( data.newOperand && data.newOperand != data.oldOperand )
			{
				delete data.oldOperand;
				*operand = data.newOperand;
			}
		}
	}

	return manipulatedTree;
}

// Manipulator.cpp