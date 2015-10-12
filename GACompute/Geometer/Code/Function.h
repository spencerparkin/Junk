// Function.h

#ifndef __Function_h__
#define __Function_h__

namespace Geometer
{
	class Function;
}

#include <lua.hpp>
#include <lauxlib.h>
#include <lualib.h>

#include <wx/object.h>
#include <wx/rtti.h>

//========================================================================
class Geometer::Function : public wxObject
{
public:

	wxDECLARE_DYNAMIC_CLASS( Function );

	static void RegisterFunctions( void );

	static bool Register( const wxClassInfo* classInfo );
	static bool Register( Function* function );
	static bool Unregister( const wxString& functionName );

	Function( void );
	virtual ~Function( void );

	virtual int Call( lua_State* L ) { return 0; }
	virtual wxString LuaFunctionName( void ) { return "?"; }

private:

	static int EntryPoint( lua_State* L );
	static int GarbageCollect( lua_State* L );
};

#endif //__Function_h__

// Function.h