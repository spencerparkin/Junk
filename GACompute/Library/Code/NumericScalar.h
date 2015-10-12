// NumericScalar.h

#pragma once

class GACOMPUTE_API GACompute::NumericScalar : public GACompute::Operand
{
public:

	NumericScalar( void );
	virtual ~NumericScalar( void );

	// Operand overrides:
	virtual void Print( char* buffer, int bufferSize, PrintFormat printFormat ) const override;
	virtual Operand* Clone( void ) const override;
	virtual Operand* Simplify( const SimplificationContext& simplificationContext ) override;
	virtual bool IsScalar( void ) const override;
	virtual bool IsAdditiveIdentity( void ) const override;
	virtual bool IsMultiplicativeIdentity( void ) const override;
	virtual void SortKey( char* buffer, int bufferSize ) const override;

	void SetReal( double realNumber );
	double GetReal( void ) const;

protected:

	double realNumber;
};

// NumericScalar.h