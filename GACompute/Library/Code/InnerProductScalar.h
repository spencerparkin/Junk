// InnerProductScalar.h

#pragma once

class GACOMPUTE_API GACompute::InnerProductScalar : public GACompute::Operand
{
public:

	InnerProductScalar( void );
	virtual ~InnerProductScalar( void );

	// Operand overrides:
	virtual void Print( char* buffer, int bufferSize, PrintFormat printFormat ) const override;
	virtual Operand* Clone( void ) const override;
	virtual Operand* Simplify( const SimplificationContext& simplificationContext ) override;
	virtual bool IsScalar( void ) const override;
	virtual bool IsAdditiveIdentity( void ) const override;
	virtual bool IsMultiplicativeIdentity( void ) const override;
	virtual void SortKey( char* buffer, int bufferSize ) const override;

	void SetNameA( const char* nameA );
	void SetNameB( const char* nameB );
	void SetNames( const char* nameA, const char* nameB );

	const char* GetNameA( void ) const;
	const char* GetNameB( void ) const;

	void SetExponent( int exponent );
	int GetExponent( void ) const;

protected:

	char* nameA;
	char* nameB;

	int exponent;
};

// InnerProductScalar.h