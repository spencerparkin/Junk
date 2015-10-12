// GAComputeLua.cpp

#include "GAComputeLua.h"
#include "GACompute.h"
#include "Functions.h"
#include "UserData.h"

static luaL_Reg gacomputelua_api[] =
{
	{ "version", l_version },
	{ "newscalar", l_newscalar },
	{ "newvector", l_newvector },
	{ "bilinear_form", l_bilinear_form },
	{ NULL, NULL },
};

int luaclose_gacomputelua( lua_State* L )
{
	GACompute::BilinearForm::Set(0);
	return 0;
}

extern "C"
{
	GACOMPUTELUA_API int luaopen_gacomputelua( lua_State* L )
	{
		luaL_newlib( L, gacomputelua_api );

		lua_newtable( L );
		lua_pushcfunction( L, &luaclose_gacomputelua );
		lua_setfield( L, -2, "__gc" );
		lua_setmetatable( L, -2 );

		return 1;
	}
}

GACOMPUTELUA_API GACompute::Operand* l_grab_operand( lua_State* L, int idx )
{
	UserData* userData = l_grabuserdata( L, idx );
	return( userData ? userData->operand : 0 );
}

// GAComputeLua.cpp