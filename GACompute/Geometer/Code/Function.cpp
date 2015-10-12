// Function.cpp

#include "Function.h"
#include "Application.h"

namespace Geometer
{
	wxIMPLEMENT_DYNAMIC_CLASS( Function, wxObject )
}

using namespace Geometer;

#define GEOMETER_FUNCTION		"Geometer.Function"

/*static*/ void Function::RegisterFunctions( void )
{
	wxClassInfo* functionClassInfo = wxClassInfo::FindClass( "Function" );
	if( !functionClassInfo )
		return;

	const wxClassInfo* classInfo = wxClassInfo::GetFirst();
	while( classInfo )
	{
		if( classInfo != functionClassInfo && classInfo->IsKindOf( functionClassInfo ) )
			Register( classInfo );

		classInfo = classInfo->GetNext();
	}
}

/*static*/ bool Function::Register( const wxClassInfo* classInfo )
{
	wxObjectConstructorFn constructorFunc = classInfo->GetConstructor();
	wxObject* object = constructorFunc();
	Function* function = wxDynamicCast( object, Function );
	if( !function )
	{
		delete object;
		return false;
	}

	return Register( function );
}

/*static*/ bool Function::Register( Function* function )
{
	lua_State* L = wxGetApp().LuaState();

	Function** userData = ( Function** )lua_newuserdata( L, sizeof( Function* ) );
	*userData = function;

	if( 1 == luaL_newmetatable( L, GEOMETER_FUNCTION ) )
	{
		lua_pushstring( L, "__gc" );
		lua_pushcfunction( L, &Function::GarbageCollect );
		lua_settable( L, -3 );
	}

	lua_setmetatable( L, -2 );
	lua_pushcclosure( L, &Function::EntryPoint, 1 );

	wxString functionName = function->LuaFunctionName();
	lua_setglobal( L, functionName );
	return true;
}

/*static*/ bool Function::Unregister( const wxString& functionName )
{
	lua_State* L =  wxGetApp().LuaState();

	lua_getglobal( L, functionName );
	Function** userData = ( Function** )luaL_testudata( L, -1, GEOMETER_FUNCTION );
	if( !userData )
	{
		lua_pop( L, 1 );
		return false;
	}

	lua_pushnil( L );
	lua_setglobal( L, functionName );
	return true;
}

Function::Function( void )
{
}

/*virtual*/ Function::~Function( void )
{
}

/*static*/ int Function::EntryPoint( lua_State* L )
{
	Function** userData = ( Function** )luaL_testudata( L, lua_upvalueindex(1), GEOMETER_FUNCTION );
	if( !userData )
		return 0;

	Function* function = *userData;
	return function->Call( L );
}

/*static*/ int Function::GarbageCollect( lua_State* L )
{
	Function** userData = ( Function** )luaL_checkudata( L, 1, GEOMETER_FUNCTION );
	if( !userData )
		return 0;

	Function* function = *userData;
	delete function;		// If we get the function from a DLL, we may need to call function->Delete() here instead.
	return 0;
}

// Function.cpp