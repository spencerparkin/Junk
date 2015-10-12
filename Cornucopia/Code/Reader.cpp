// Reader.cpp

#include "Cornucopia.h"

using namespace Cornucopia;

//=================================================================================
Reader::Reader( const std::string& file, std::string* buffer /*= 0*/ )
{
	chunk = 0;
	fileHandle = INVALID_HANDLE_VALUE;
	this->file = file;
	this->buffer = buffer;
	errorCode = ERROR_SUCCESS;
}

//=================================================================================
Reader::~Reader( void )
{
	if( fileHandle != INVALID_HANDLE_VALUE )
		CloseHandle( fileHandle );

	if( chunk )
		delete[] chunk;
}

//=================================================================================
/*static*/ const char* Reader::Read( lua_State* L, void* data, size_t* size )
{
	Reader* reader = ( Reader* )data;
	return reader->ReadChunk( L, size );
}

//=================================================================================
// As of this writing, this reader is not yet suffisticated enough
// to read in a Lua file over multiple chunk reads.  We will either
// read it in all at once, or we will fail entirely.
const char* Reader::ReadChunk( lua_State* L, size_t* size )
{
	*size = 0;

	// If we already made one attempt, then we're done.
	// That attempt either worked or it didn't.
	if( chunk )
		return 0;

	// Are we reading from a file?
	if( file.empty() && buffer )
	{
		// No.  We're reading from a buffer.
		*size = buffer->size() + 1;
		chunk = new char[ *size ];
		strcpy_s( ( char* )chunk, *size, buffer->c_str() );
		chunk[ *size - 1 ] = '\r';
		return chunk;
	}

	// Yes.  Try to open the file for reading.
	fileHandle = CreateFileA( file.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if( fileHandle == INVALID_HANDLE_VALUE )
	{
		errorCode = GetLastError();
		return 0;
	}

	// Ascertain the size of the file.
	DWORD sizeHi, sizeLo;
	sizeLo = GetFileSize( fileHandle, &sizeHi );
	if( sizeHi != 0 )
		return 0;		// I'm not yet going to deal with files this big.
	
	// Allocate enough memory for the entire file.
	chunk = new char[ sizeLo ];
	if( !chunk )
		return 0;

	// Try to read in the entire file all at once.
	DWORD bytesRead = 0;
	if( !ReadFile( fileHandle, chunk, sizeLo, &bytesRead, NULL ) )
	{
		errorCode = GetLastError();
		return 0;
	}
	if( bytesRead != sizeLo )
		return 0;

	// Success!  Tell the caller how big and return the chunk data.
	*size = ( size_t )sizeLo;
	return chunk;
}

//=================================================================================
DWORD Reader::GetErrorCode( void )
{
	return errorCode;
}

// Reader.cpp