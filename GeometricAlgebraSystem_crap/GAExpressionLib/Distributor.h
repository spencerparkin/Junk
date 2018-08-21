// Distributor.h

#pragma once

#include "Manipulator.h"

namespace GAExpressionLib
{
	class Distributor;
}

class GA_EXPRESSION_LIB_API GAExpressionLib::Distributor : public GAExpressionLib::Manipulator
{
public:

	Distributor( void );
	virtual ~Distributor( void );

	// Overrides:
	virtual bool Manipulate( ManipulationData& data ) override;
};

// Distributor.h