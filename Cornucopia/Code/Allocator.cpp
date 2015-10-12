// Allocator.cpp

#include "Cornucopia.h"

//=================================================================================
// This implementation is barrowed from lauxlib.c.
// Interestingly, the linker can't find this unless we
// fully qualify it with the name-space, even if we're
// using the right name-space.
void* Cornucopia::Allocate( void* ud, void* ptr, size_t osize, size_t nsize )
{
	if( nsize == 0 )
	{
		free( ptr );
		return 0;
	}

	return realloc( ptr, nsize );
}

// Allocator.cpp