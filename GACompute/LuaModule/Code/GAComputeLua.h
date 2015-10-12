// GAComputeLua.h

#pragma once

#if defined EXPORT_GACOMPUTELUA
#	define GACOMPUTELUA_API		__declspec( dllexport )
#elif defined IMPORT_GACOMPUTELUA
#	define GACOMPUTELUA_API		__declspec( dllimport )
#else
#	define GACOMPUTELUA_API
#endif

#include <lua.hpp>
#include "GACompute.h"

extern "C"
{
	extern GACOMPUTELUA_API int luaopen_gacomputelua( lua_State* L );
}

GACOMPUTELUA_API GACompute::Operand* l_grab_operand( lua_State* L, int idx );

// GAComputeLua.h