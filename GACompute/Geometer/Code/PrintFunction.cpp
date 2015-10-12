// PrintFunction.cpp

#include "PrintFunction.h"
#include "LuaIdioms.h"

namespace Geometer
{
	wxIMPLEMENT_DYNAMIC_CLASS( PrintFunction, Function )
}

using namespace Geometer;

PrintFunction::PrintFunction( void )
{
}

/*virtual*/ PrintFunction::~PrintFunction( void )
{
}

/*virtual*/ int PrintFunction::Call( lua_State* L )
{
	const char* string = luaL_checkstring( L, 1 );
	if( !string )
		return 0;

	if( !l_getornewtable( L, "console", true ) )
		return 0;

	if( !l_getornewtable( L, "output" ) )
		return 0;

	int length = lua_rawlen( L, -1 );
	lua_pushinteger( L, length + 1 );
	lua_pushvalue( L, 1 );
	lua_settable( L, -3 );
	return 0;
}

/*virtual*/ wxString PrintFunction::LuaFunctionName( void )
{
	return "print";
}

// PrintFunction.cpp