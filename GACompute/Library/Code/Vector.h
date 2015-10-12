// Vector.h

#pragma once

class GACOMPUTE_API GACompute::Vector : public GACompute::Operand
{
public:

	Vector( void );
	virtual ~Vector( void );

	// Operand overrides:
	virtual void Print( char* buffer, int bufferSize, PrintFormat printFormat ) const override;
	virtual Operand* Clone( void ) const override;
	virtual Operand* Simplify( const SimplificationContext& simplificationContext ) override;
	virtual bool IsScalar( void ) const override;
	virtual bool IsAdditiveIdentity( void ) const override;
	virtual void SortKey( char* buffer, int bufferSize ) const override;

	void SetName( const char* name );
	const char* GetName( void ) const;

	void SetScalar( Operand* scalar, bool deleteExisting = true );
	Operand* GetScalar( void );
	const Operand* GetScalar( void ) const;

	void MarryWithScalar( Operand* scalar );

private:

	char* name;

	Operand* scalar;
};

// Vector.h