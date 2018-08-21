// ScaledSymbol.h

#pragma once

#include "GAExpLibApi.h"

namespace GAExpressionLib
{
	class ScaledSymbol;
	class Operand;
}

class GA_EXPRESSION_LIB_API GAExpressionLib::ScaledSymbol
{
public:

	ScaledSymbol( void );
	virtual ~ScaledSymbol( void );

	char* name;
	Operand* scalar;
};

// ScaledSymbol.h