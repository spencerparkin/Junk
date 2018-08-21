// Operand.cpp

#include "Operand.h"

using namespace GAExpressionLib;

Operand::Operand( void )
{
}

/*virtual*/ Operand::~Operand( void )
{
}

/*virtual*/ bool Operand::IsScalar( void ) const
{
	// Returning false here means, "we don't know."
	return false;
}

// Operand.cpp