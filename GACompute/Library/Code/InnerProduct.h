// InnerProduct.h

#pragma once

class GACOMPUTE_API GACompute::InnerProduct : public GACompute::Operation
{
public:

	InnerProduct( void );
	virtual ~InnerProduct( void );

	// Operand overrides:
	virtual Operand* Simplify( const SimplificationContext& simplificationContext ) override;

	// Operation overrides:
	virtual Operand* Create( void ) const override;
	virtual Operand* CreateVacuousCase( void ) const;
	virtual bool TakesPrecedenceOver( const Operation* operation ) const;
	virtual bool CanDistributeOver( const Operation* operation ) const override;
	virtual void PrintWhenEmpty( char* buffer, int bufferSize, PrintFormat printFormat ) const override;
	virtual const char* OperatorSymbol( PrintFormat printFormat ) const override;
	virtual bool IsAdditiveIdentity( void ) const override;

private:

	enum DotOrder
	{
		VECTOR_DOT_BLADE,
		BLADE_DOT_VECTOR,
	};

	Sum* GenerateAlternatingSeries( const Vector* vector, OuterProduct* blade, DotOrder dotOrder );

	GeometricProduct* GenerateScalarProduct( const Vector* vectorA, const Vector* vectorB );
};

// InnerProduct.h