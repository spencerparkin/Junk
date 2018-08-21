// ScaledSymbol.cpp

#include "ScaledSymbol.h"
#include "Operand.h"
#include <malloc.h>
#include <string.h>

using namespace GAExpressionLib;

ScaledSymbol::ScaledSymbol( void )
{
	name = nullptr;
	scalar = nullptr;
}

/*virtual*/ ScaledSymbol::~ScaledSymbol( void )
{
	if( name )
		free( name );

	delete scalar;
}

// ScaledSymbol.cpp