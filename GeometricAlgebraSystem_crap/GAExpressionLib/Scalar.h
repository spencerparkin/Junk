// Scalar.h

#pragma once

#include "Operand.h"

namespace GAExpressionLib
{
	class Scalar;
}

class GA_EXPRESSION_LIB_API GAExpressionLib::Scalar : public GAExpressionLib::Operand
{
public:

	Scalar( void );
	virtual ~Scalar( void );

	// Overrides:
	virtual bool IsScalar( void ) const override;
};

// Scalar.h