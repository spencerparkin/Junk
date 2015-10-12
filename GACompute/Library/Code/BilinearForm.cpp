// BilinearForm.cpp

#include "GACompute.h"

using namespace GACompute;

BilinearForm* BilinearForm::theBilinearForm = 0;

BilinearForm::BilinearForm( void )
{
}

/*virtual*/ BilinearForm::~BilinearForm( void )
{
}

/*static*/ void BilinearForm::Set( BilinearForm* bilinearForm, bool deleteExisting /*= true*/ )
{
	if( deleteExisting )
		delete theBilinearForm;

	theBilinearForm = bilinearForm;
}

/*static*/ BilinearForm* BilinearForm::Get( void )
{
	return theBilinearForm;
}

// BilinearForm.cpp