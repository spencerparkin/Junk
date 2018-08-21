// Compressor.h

#pragma once

#include "Manipulator.h"
#include "Operation.h"

namespace GAExpressionLib
{
	class Compressor;
}

// Most, if not all, manipulation algorithms are supposed to be decoupled
// from the expression tree data-structure, but in this case, it was too
// convenient to delegate certain tasks to virtual methods of operands.
class GA_EXPRESSION_LIB_API GAExpressionLib::Compressor : public GAExpressionLib::Manipulator
{
public:

	Compressor( void );
	virtual ~Compressor( void );

protected:

	virtual bool Manipulate( ManipulationData& data ) override;

public:

	bool Compress( Operation* operation );

	virtual bool Combine( const Operand* leftOperand, const Operand* rightOperand, const Operation* operation, Operand** resultOperand );

private:

	struct Pair
	{
		Pair( Operation::OperandList::Node* leftNode = nullptr, Operation::OperandList::Node* rightNode = nullptr )
		{
			this->leftNode = leftNode;
			this->rightNode = rightNode;
		}

		Operation::OperandList::Node* leftNode;
		Operation::OperandList::Node* rightNode;
	};

	typedef List< Pair, DumbDataManager< Pair > > PairQueue;
	PairQueue pairQueue;

	void CullQueuedPairsContaining( Operation::OperandList::Node* node );
};

// Compressor.h