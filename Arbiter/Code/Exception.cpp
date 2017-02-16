// Exception.cpp

#include "Exception.h"

Exception::Exception( const wxString& error )
{
	this->error = error;
}

/*virtual*/ Exception::~Exception( void )
{
}

void Exception::Log( void )
{
	// TODO: Add to exceptions log.
}

// Exception.cpp