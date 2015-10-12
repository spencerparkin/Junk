// NumericReal.h

#pragma once

#include "Scalar.h"

namespace GAExpressionLib
{
	class NumericReal;
}

class GA_EXPRESSION_LIB_API GAExpressionLib::NumericReal : public GAExpressionLib::Scalar
{
public:

	NumericReal( void );
	virtual ~NumericReal( void );

	// Overrides:
	virtual bool IsSameTypeAs( const Operand* operand ) const override;
	virtual Operand* Clone( void ) const override;
	virtual void Print( char* buffer, int bufferSize, PrintFormat format ) const override;

	double real;
};

// NumericReal.h