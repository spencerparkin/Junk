// UserData.cpp

#include "UserData.h"
#include "UserDataOverload.h"
#include <stdlib.h>
#include <stdio.h>

static const char* userDataMetaTableKey = "__gacomputelua_userdata_metatable__";

static const char* magicKey = "gacomputelua";

UserData* l_newuserdata( lua_State* L )
{
	size_t size = sizeof( UserData );
	UserData* userData = ( UserData* )lua_newuserdata( L, size );
	if( !userData )
		return 0;

	strcpy_s( userData->magicKey, sizeof( userData->magicKey ), magicKey );
	
	userData->operand = 0;

	lua_pushglobaltable( L );		// Should we use the registry instead?
	lua_getfield( L, -1, userDataMetaTableKey );

	if( lua_isnil( L, -1 ) )
	{
		lua_pop( L, 1 );
		lua_newtable( L );

		lua_pushcfunction( L, &l_deleteuserdata );
		lua_setfield( L, -2, "__gc" );
		lua_pushcfunction( L, &l_userdata_add );
		lua_setfield( L, -2, "__add" );
		lua_pushcfunction( L, &l_userdata_sub );
		lua_setfield( L, -2, "__sub" );
		lua_pushcfunction( L, &l_userdata_mul );
		lua_setfield( L, -2, "__mul" );
		lua_pushcfunction( L, &l_userdata_mod );
		lua_setfield( L, -2, "__mod" );
		lua_pushcfunction( L, &l_userdata_concat );
		lua_setfield( L, -2, "__concat" );
		lua_pushcfunction( L, &l_userdata_pow );
		lua_setfield( L, -2, "__pow" );
		lua_pushcfunction( L, &l_userdata_div );
		lua_setfield( L, -2, "__div" );
		lua_pushcfunction( L, &l_userdata_index );
		lua_setfield( L, -2, "__index" );
		lua_pushcfunction( L, &l_userdata_newindex );
		lua_setfield( L, -2, "__newindex" );
		lua_pushcfunction( L, &l_userdata_len );
		lua_setfield( L, -2, "__len" );
		lua_pushcfunction( L, &l_userdata_unm );
		lua_setfield( L, -2, "__unm" );
		lua_pushcfunction( L, &l_userdata_eq );
		lua_setfield( L, -2, "__eq" );
		lua_pushcfunction( L, &l_userdata_tostring );
		lua_setfield( L, -2, "__tostring" );

		lua_setfield( L, -2, userDataMetaTableKey );
		lua_getfield( L, -1, userDataMetaTableKey );
	}

	lua_setmetatable( L, -3 );
	lua_pop( L, 1 );

	return userData;
}

UserData* l_grabuserdata( lua_State* L, int idx )
{
	UserData* userData = 0;

	if( lua_isuserdata( L, idx ) )
	{
		userData = ( UserData* )lua_touserdata( L, idx );
		if( userData )
		{
			int len = strlen( magicKey );
			char data[16];
			strncpy_s( data, sizeof( data ), userData->magicKey, len );
			data[ len ] = '\0';
			if( 0 != strcmp( data, magicKey ) )
				userData = 0;
		}
	}

	return userData;
}

int l_deleteuserdata( lua_State* L )
{
	UserData* userData = l_grabuserdata( L, -1 );
	if( userData )
	{
		delete userData->operand;
		userData->operand = 0;
	}

	return 0;
}

// UserData.cpp