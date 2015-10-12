// ClearFunction.cpp

#include "ClearFunction.h"
#include "LuaIdioms.h"

namespace Geometer
{
	wxIMPLEMENT_DYNAMIC_CLASS( ClearFunction, Function )
}

using namespace Geometer;

ClearFunction::ClearFunction( void )
{
}

/*virtual*/ ClearFunction::~ClearFunction( void )
{
}

/*virtual*/ int ClearFunction::Call( lua_State* L )
{
	if( !l_getornewtable( L, "console", true ) )
		return 0;

	if( !l_getornewtable( L, "output" ) )
		return 0;

	int length = lua_rawlen( L, -1 );
	if( length == 0 )
		return 0;

	for( int i = 1; i <= length; i++ )
	{
		lua_pushinteger( L, i );
		lua_pushnil( L );
		lua_settable( L, -3 );
	}

	lua_pushboolean( L, 1 );
	lua_setfield( L, -2, "clear" );

	return 0;
}

/*virtual*/ wxString ClearFunction::LuaFunctionName( void )
{
	return "clear";
}

// ClearFunction.cpp