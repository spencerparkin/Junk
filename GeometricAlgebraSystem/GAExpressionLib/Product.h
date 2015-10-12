// Product.h

#pragma once

#include "Operation.h"

namespace GAExpressionLib
{
	class Product;
	class ScaledSymbol;
}

class GA_EXPRESSION_LIB_API GAExpressionLib::Product : public GAExpressionLib::Operation
{
public:

	Product( void );
	virtual ~Product( void );
	
	// Overrides:
	virtual bool Commutes( void ) const override;
	virtual Operand* Identity( void ) const override;
	virtual bool Combine( const Operand* leftOperand, const Operand* rightOperand, Operand** resultOperand ) const override;
	virtual void PrintIdentity( char* buffer, int bufferSize, PrintFormat format ) const override;

	bool HasAScalar( void ) const;
	bool IsHomogeneousOfScalars( void ) const;
	bool IsHomogeneousOfVectors( void ) const;
	bool IsHomogeneousOfScaledSymbols( void ) const;
	bool IsHomogeneousOfSymbolicReals( void ) const;
	bool IsHomogeneousOfBlades( int minimumGrade ) const;
	bool IsHomogeneousOfVersors( int minimumGrade ) const;

	int CountScaledSymbols( void ) const;
	ScaledSymbol* FindLeadingScaledSymbol( void );
	const ScaledSymbol* FindLeadingScaledSymbol( void ) const;
	void StripScalars( OperandList& operandList );
	void CloneScalars( OperandList& scalarList ) const;
	bool ConsolidateScalars( void );
};

// Product.h