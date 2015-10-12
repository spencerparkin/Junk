// Operation.h

#pragma once

#include "Operand.h"
#include "List.h"

namespace GAExpressionLib
{
	class Operation;
	class Atom;
}

class GA_EXPRESSION_LIB_API GAExpressionLib::Operation : public GAExpressionLib::Operand
{
public:

	Operation( void );
	virtual ~Operation( void );

	// Interface:
	virtual bool AssociatesWith( const Operation* operation ) const = 0;
	virtual bool DistributesOver( const Operation* operation ) const = 0;
	virtual bool Commutes( void ) const = 0;
	virtual bool TakesPrecedenceOver( const Operation* operation ) const = 0;
	virtual void PrintBinaryOperator( char* buffer, int bufferSize, PrintFormat format ) const = 0;
	virtual void PrintIdentity( char* buffer, int bufferSize, PrintFormat format ) const = 0;
	virtual Operand* Identity( void ) const = 0;
	virtual Operand* Allocate( void ) const = 0;
	virtual bool Combine( const Operand* leftOperand, const Operand* rightOperand, Operand** resultOperand ) const = 0;
	virtual bool Sort( void );

	// Overrides:
	virtual Operand* Clone( void ) const override;
	virtual void Print( char* buffer, int bufferSize, PrintFormat format ) const override;
	virtual bool IsScalar( void ) const override;

	int SortByPrint( void );
	static int CompareByOperandPrint( const Operand* leftOperand, const Operand* rightOperand );

	typedef List< Operand* > OperandList;
	OperandList operandList;
};

// Operation.h