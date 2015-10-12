// Operand.cpp

#include "GACompute.h"

using namespace GACompute;

Operand::Operand( void )
{
}

/*virtual*/ Operand::~Operand( void )
{
}

/*static*/ bool Operand::Simplify( Operand*& operand, const SimplificationContext& simplificationContext )
{
	Operand* simplifiedOperand = 0;

	try
	{
		simplifiedOperand = operand->Simplify( simplificationContext );
	}
	catch( const Error* error )
	{
		Error::Handle( error );
		return false;
	}

	if( !simplifiedOperand )
		return false;

	if( simplifiedOperand != operand )
	{
		delete operand;
		operand = simplifiedOperand;
	}

	return true;
}

/*static*/ bool Operand::CompletelySimplify( Operand*& operand, const SimplificationContext& simplificationContext )
{
	while( Simplify( operand, simplificationContext ) )
	{
	}

	return true;
}

/*virtual*/ bool Operand::IsScalar( void ) const
{
	return false;
}

/*virtual*/ bool Operand::IsBlade( void ) const
{
	return false;
}

/*virtual*/ bool Operand::IsVersor( void ) const
{
	return false;
}

/*virtual*/ bool Operand::IsAdditiveIdentity( void ) const
{
	return false;
}

/*virtual*/ bool Operand::IsMultiplicativeIdentity( void ) const
{
	return false;
}

// Operand.cpp