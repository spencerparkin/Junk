// OuterProduct.h

#pragma once

class GACOMPUTE_API GACompute::OuterProduct : public GACompute::Operation
{
public:

	OuterProduct( void );
	virtual ~OuterProduct( void );

	// Operand overrides:
	virtual Operand* Simplify( const SimplificationContext& simplificationContext ) override;
	virtual bool IsBlade( void ) const override;

	// Operation overrides:
	virtual Operand* Create( void ) const override;
	virtual Operand* CreateVacuousCase( void ) const;
	virtual bool TakesPrecedenceOver( const Operation* operation ) const;
	virtual bool CanDistributeOver( const Operation* operation ) const override;
	virtual bool CanAssociateWith( const Operation* operation ) const override;
	virtual void PrintWhenEmpty( char* buffer, int bufferSize, PrintFormat printFormat ) const override;
	virtual const char* OperatorSymbol( PrintFormat printFormat ) const override;
	virtual bool IsAdditiveIdentity( void ) const override;
	virtual int Sort( void ) override;

private:

	bool ShouldExpand( const SimplificationContext& simplificationContext ) const;
};

// OuterProduct.h