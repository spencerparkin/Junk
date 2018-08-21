// GeometricProduct.h

#pragma once

#include "Product.h"

namespace GAExpressionLib
{
	class GeometricProduct;
}

class GA_EXPRESSION_LIB_API GAExpressionLib::GeometricProduct : public GAExpressionLib::Product
{
public:

	GeometricProduct( void );
	virtual ~GeometricProduct( void );

	// Overrides:
	virtual bool IsSameTypeAs( const Operand* operand ) const override;
	virtual bool AssociatesWith( const Operation* operation ) const override;
	virtual bool DistributesOver( const Operation* operation ) const override;
	virtual bool TakesPrecedenceOver( const Operation* operation ) const override;
	virtual void PrintBinaryOperator( char* buffer, int bufferSize, PrintFormat format ) const override;
	virtual Operand* Allocate( void ) const override;
	virtual bool Combine( const Operand* leftOperand, const Operand* rightOperand, Operand** resultOperand ) const override;
	virtual bool Sort( void ) override;
};

// GeometricProduct.h