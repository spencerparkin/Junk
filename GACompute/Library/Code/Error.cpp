// Error.cpp

#include "GACompute.h"

using namespace GACompute;

Error::Handler* Error::errorHandler = 0;

Error::Error( void )
{
	string = 0;
}

/*virtual*/ Error::~Error( void )
{
	delete[] string;
}

const char* Error::GetString( void ) const
{
	return string;
}

void Error::Format( const char* format, ... )
{
	delete[] string;

	char buffer[ 1024 ];
	va_list args;
	va_start( args, format );
	vsnprintf_s( buffer, sizeof( buffer ), _TRUNCATE, format, args );
	va_end( args );

	int len = strlen( buffer ) + 1;
	string = new char[ len ];
	strcpy_s( string, len, buffer );
}

/*static*/ void Error::Handle( const Error* error )
{
	if( errorHandler )
		errorHandler->Handle( error );

	delete error;
}

/*static*/ void Error::SetHandler( Handler* handler, bool deleteExisting /*= true*/ )
{
	if( deleteExisting )
		delete errorHandler;

	errorHandler = handler;
}

/*static*/ Error::Handler* Error::GetHandler( void )
{
	return errorHandler;
}

// Error.cpp