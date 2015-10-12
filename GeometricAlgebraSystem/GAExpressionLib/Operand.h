// Operand.h

#pragma once

#include "GAExpLibApi.h"

namespace GAExpressionLib
{
	class Operand;
}

class GA_EXPRESSION_LIB_API GAExpressionLib::Operand
{
public:

	enum PrintFormat { PRINT_NORMAL, PRINT_LATEX, PRINT_SORT };

	Operand( void );
	virtual ~Operand( void );

	// Interface:
	virtual bool IsSameTypeAs( const Operand* ) const = 0;
	virtual Operand* Clone( void ) const = 0;
	virtual void Print( char* buffer, int bufferSize, PrintFormat format ) const = 0;
	virtual bool IsScalar( void ) const;
	//virtual void Evaluate( Multivector& resultMultivector ) const = 0;
};

// Operand.h