// Sum.h

#pragma once

class GACOMPUTE_API GACompute::Sum : public GACompute::Operation
{
public:

	Sum( void );
	virtual ~Sum( void );

	// Operand overrides:
	virtual Operand* Simplify( const SimplificationContext& simplificationContext ) override;

	// Operation overrides:
	virtual Operand* Create( void ) const override;
	virtual Operand* CreateVacuousCase( void ) const;
	virtual bool TakesPrecedenceOver( const Operation* operation ) const;
	virtual bool CanAssociateWith( const Operation* operation ) const override;
	virtual void PrintWhenEmpty( char* buffer, int bufferSize, PrintFormat printFormat ) const override;
	virtual const char* OperatorSymbol( PrintFormat printFormat ) const override;
	virtual bool IsAdditiveIdentity( void ) const override;

	bool CullZeroOperands( void );
	bool CombineLikeOperands( void );
	bool OperationsAlike( const Operation* operationA, const Operation* operationB );
	Operand* AddScalars( const Operand* operandA, const Operand* operandB );
};

// Sum.h