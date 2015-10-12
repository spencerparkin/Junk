// Error.cpp

#include "Cornucopia.h"

using namespace Cornucopia;

//=================================================================================
Error::Error( const char* errorMessageFormat, ... )
{
	va_list args;
	va_start( args, errorMessageFormat );
	FormatErrorMessage( errorMessageFormat, args );
	va_end( args );
}

//=================================================================================
Error::Error( const char* errorMessageFormat, va_list args )
{
	FormatErrorMessage( errorMessageFormat, args );
}

//=================================================================================
/*virtual*/ Error::~Error( void )
{
}

//=================================================================================
const std::string& Error::ErrorMessage( void ) const
{
	return errorMessage;
}

//=================================================================================
void Error::FormatErrorMessage( const char* errorMessageFormat, va_list args )
{
	char formattedMessage[ 256 ];
	vsprintf_s( formattedMessage, sizeof( formattedMessage ), errorMessageFormat, args );
	errorMessage = formattedMessage;
}

// Error.cpp