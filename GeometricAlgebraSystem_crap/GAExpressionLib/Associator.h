// Associator.h

#pragma once

#include "Manipulator.h"

namespace GAExpressionLib
{
	class Associator;
}

class GA_EXPRESSION_LIB_API GAExpressionLib::Associator : public GAExpressionLib::Manipulator
{
public:

	Associator( void );
	virtual ~Associator( void );

	// Overrides:
	virtual bool Manipulate( ManipulationData& data ) override;
};

// Associator.h