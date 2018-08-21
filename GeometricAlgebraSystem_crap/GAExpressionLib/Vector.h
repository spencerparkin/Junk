// Vector.h

#pragma once

#include "Operand.h"
#include "ScaledSymbol.h"

namespace GAExpressionLib
{
	class Vector;
}

class GA_EXPRESSION_LIB_API GAExpressionLib::Vector : public GAExpressionLib::Operand, public GAExpressionLib::ScaledSymbol
{
public:

	Vector( void );
	virtual ~Vector( void );

	// Overrides:
	virtual bool IsSameTypeAs( const Operand* operand ) const override;
	virtual Operand* Clone( void ) const override;
	virtual void Print( char* buffer, int bufferSize, PrintFormat format ) const override;
};

// Vector.h