// Operand.h

#pragma once

class GACOMPUTE_API GACompute::Operand
{
public:

	Operand( void );
	virtual ~Operand( void );

	enum PrintFormat
	{
		PRINT_NORMAL,
		PRINT_LATEX,
	};

	struct SimplificationContext
	{
		enum Target
		{
			SUM_OF_BLADES,
			SUM_OF_VERSORS,
		};

		Target target;
		const Operation* operation;
	};

	// Mandatory overrides:
	virtual void Print( char* buffer, int bufferSize, PrintFormat printFormat ) const = 0;
	virtual Operand* Clone( void ) const = 0;
	virtual Operand* Simplify( const SimplificationContext& simplificationContext ) = 0;
	virtual void SortKey( char* buffer, int bufferSize ) const = 0;

	// Optional overrides:
	virtual bool IsScalar( void ) const;
	virtual bool IsBlade( void ) const;
	virtual bool IsVersor( void ) const;
	virtual bool IsAdditiveIdentity( void ) const;
	virtual bool IsMultiplicativeIdentity( void ) const;	// This is with respect to the geometric product, which is the only invertible product.

	// Here's an idea for later; much later...
	//virtual bool Evaluate( ThirdPartyMultivector& result ) const = 0;

	static bool Simplify( Operand*& operand, const SimplificationContext& simplificationContext );
	static bool CompletelySimplify( Operand*& operand, const SimplificationContext& simplificationContext );
};

// Operand.h