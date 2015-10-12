// SymbolicScalar.h

#pragma once

class GACOMPUTE_API GACompute::SymbolicScalar : public GACompute::Operand
{
public:

	SymbolicScalar( void );
	virtual ~SymbolicScalar( void );

	// Operand overrides:
	virtual void Print( char* buffer, int bufferSize, PrintFormat printFormat ) const override;
	virtual Operand* Clone( void ) const override;
	virtual Operand* Simplify( const SimplificationContext& simplificationContext ) override;
	virtual bool IsScalar( void ) const override;
	virtual bool IsAdditiveIdentity( void ) const override;
	virtual bool IsMultiplicativeIdentity( void ) const override;
	virtual void SortKey( char* buffer, int bufferSize ) const override;

	void SetName( const char* name );
	const char* GetName( void ) const;

	void SetExponent( int exponent );
	int GetExponent( void ) const;

protected:

	char* name;

	int exponent;
};

// SymbolicScalar.h