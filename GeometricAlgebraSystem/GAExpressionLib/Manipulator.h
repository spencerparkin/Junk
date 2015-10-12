// Manipulator.h

#pragma once

#include "Operand.h"

namespace GAExpressionLib
{
	class Manipulator;
}

class GA_EXPRESSION_LIB_API GAExpressionLib::Manipulator
{
public:

	Manipulator( void );
	virtual ~Manipulator( void );

	struct ManipulationData
	{
		Operand* oldOperand;
		Operand* newOperand;
	};

	// Interface:
	virtual bool ManipulateTree( Operand** operand );

protected:

	virtual bool Manipulate( ManipulationData& data );

	bool ManipulateRecursively( Operand** operand );
};

// Manipulator.h