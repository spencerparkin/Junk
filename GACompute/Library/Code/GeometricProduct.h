// GeometricProduct.h

#pragma once

class GACOMPUTE_API GACompute::GeometricProduct : public GACompute::Operation
{
public:

	GeometricProduct( void );
	virtual ~GeometricProduct( void );

	// Operand overrides:
	virtual Operand* Simplify( const SimplificationContext& simplificationContext ) override;
	virtual bool IsVersor( void ) const override;

	// Operation overrides:
	virtual Operand* Create( void ) const override;
	virtual Operand* CreateVacuousCase( void ) const;
	virtual bool TakesPrecedenceOver( const Operation* operation ) const;
	virtual bool CanDistributeOver( const Operation* operation ) const override;
	virtual bool CanAssociateWith( const Operation* operation ) const override;
	virtual void PrintWhenEmpty( char* buffer, int bufferSize, PrintFormat printFormat ) const override;
	virtual const char* OperatorSymbol( PrintFormat printFormat ) const override;
	virtual bool IsAdditiveIdentity( void ) const override;
	virtual bool IsMultiplicativeIdentity( void ) const override;

private:

	bool CombineOperands( void );
	bool CullOneOperands( void );
	Operand* MultiplyScalars( const Operand* operandA, const Operand* operandB );
	bool ShouldExpand( const SimplificationContext& simplificationContext ) const;
};

// GeometricProduct.h