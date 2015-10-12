// GAExpLuaApi.cpp

#include "GAExpLuaApi.h"
#include "Expression.h"
#include "BilinearFormLua.h"
#include <string.h>

// Operands:
#include <NumericReal.h>
#include <SymbolicReal.h>
#include <Vector.h>

// Manipulators:
#include <Expander.h>

static luaL_Reg gaexplua_api[] =
{
	{ "new_scalar", l_new_scalar },
	{ "new_vector", l_new_vector },
	{ "bilinear_form", l_bilinear_form },
	{ NULL, NULL },
};

extern "C"
{
	GA_EXPRESSION_LUA_API int luaopen_GAExpressionLua( lua_State* L )
	{
		luaL_newlib( L, gaexplua_api );

		lua_newtable( L );
		lua_pushcfunction( L, &luaclose_GAExpressionLua );
		lua_setfield( L, -2, "__gc" );
		lua_setmetatable( L, -2 );

		lua_pushinteger( L, GAExpressionLib::Expander::SUM_OF_BLADES );
		lua_setfield( L, -2, "SUM_OF_BLADES" );

		lua_pushinteger( L, GAExpressionLib::Expander::SUM_OF_VERSORS );
		lua_setfield( L, -2, "SUM_OF_VERSORS" );

		return 1;
	}
}

int luaclose_GAExpressionLua( lua_State* L )
{
	using namespace GAExpressionLib;

	if( bilinearForm == bilinearFormLua )
		bilinearForm = nullptr;

	delete bilinearFormLua;
	bilinearFormLua = nullptr;

	return 0;
}

int l_new_scalar( lua_State* L )
{
	using namespace GAExpressionLib;

	Expression* exp = l_new_exp( L );
	if( !exp )
		return luaL_error( L, "Failed to create new expression." );

	exp->operand = l_grab_real( L, -2 );
	if( !exp->operand )
	{
		lua_pop( L, 1 );
		lua_pushnil( L );
	}
	
	return 1;
}

int l_new_vector( lua_State* L )
{
	using namespace GAExpressionLib;

	Expression* exp = l_new_exp( L );
	if( !exp )
		return luaL_error( L, "Failed to create new expression." );

	if( lua_isstring( L, -2 ) )
	{
		Vector* vector = new Vector();
		vector->name = _strdup( lua_tostring( L, -2 ) );
		exp->operand = vector;
	}
	
	if( !exp->operand )
	{
		lua_pop( L, 1 );
		lua_pushnil( L );
	}

	return 1;
}

int l_bilinear_form( lua_State* L )
{
	using namespace GAExpressionLib;

	if( !lua_isfunction( L, -1 ) )
		return luaL_error( L, "Expected a function." );

	if( bilinearForm && bilinearFormLua != bilinearForm )
		return 0;

	if( !bilinearFormLua )
		bilinearFormLua = new BilinearFormLua( L );

	if( bilinearForm )
		delete bilinearForm;

	bilinearForm = bilinearFormLua;

	lua_pushlightuserdata( L, ( void* )&BilinearFormLua::registryKey );
	lua_pushvalue( L, -2 );
	lua_settable( L, LUA_REGISTRYINDEX );

	return 0;
}

// GAExpLuaApi.cpp