// SymbolicReal.h

#pragma once

#include "Scalar.h"
#include "ScaledSymbol.h"

namespace GAExpressionLib
{
	class SymbolicReal;
}

class GA_EXPRESSION_LIB_API GAExpressionLib::SymbolicReal : public GAExpressionLib::Scalar, public GAExpressionLib::ScaledSymbol
{
public:

	SymbolicReal( void );
	virtual ~SymbolicReal( void );

	// Overrides:
	virtual bool IsSameTypeAs( const Operand* operand ) const override;
	virtual Operand* Clone( void ) const override;
	virtual void Print( char* buffer, int bufferSize, PrintFormat format ) const override;

	int exponent;
};

// SymbolicReal.h