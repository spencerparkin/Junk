// BilinearForm.h

#pragma once

class GACOMPUTE_API GACompute::BilinearForm
{
public:

	BilinearForm( void );
	virtual ~BilinearForm( void );

	virtual bool Evaluate( const char* nameA, const char* nameB, double& result ) const = 0;

	static void Set( BilinearForm* bilinearForm, bool deleteExisting = true );
	static BilinearForm* Get( void );

private:

	static BilinearForm* theBilinearForm;
};

// BilinearForm.h