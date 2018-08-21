// Addition.h

#pragma once

#include "Operation.h"

namespace GAExpressionLib
{
	class Addition;
}

class GA_EXPRESSION_LIB_API GAExpressionLib::Addition : public GAExpressionLib::Operation
{
public:

	Addition( void );
	virtual ~Addition( void );

	// Overrides:
	virtual bool IsSameTypeAs( const Operand* operand ) const override;
	virtual bool Commutes( void ) const override;
	virtual bool AssociatesWith( const Operation* operation ) const override;
	virtual bool DistributesOver( const Operation* operation ) const override;
	virtual bool TakesPrecedenceOver( const Operation* operation ) const override;
	virtual void PrintBinaryOperator( char* buffer, int bufferSize, PrintFormat format ) const override;
	virtual void PrintIdentity( char* buffer, int bufferSize, PrintFormat format ) const override;
	virtual Operand* Identity( void ) const override;
	virtual Operand* Allocate( void ) const override;
	virtual bool Combine( const Operand* leftOperand, const Operand* rightOperand, Operand** resultOperand ) const override;
};

// Addition.h