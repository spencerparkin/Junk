// Application.h

#ifndef __Application_h__
#define __Application_h__

#include <wx/setup.h>
#include <wx/app.h>
#include <wx/hashmap.h>
#include <GL/gl.h>
#include <lua.hpp>
#include <lauxlib.h>
#include <lualib.h>
#include "Frame.h"
#include "GACompute.h"

namespace Geometer
{
	class Application;
	class Geometry;

	WX_DECLARE_HASH_MAP( GLuint, Geometry*, wxIntegerHash, wxIntegerEqual, GeometryMap );
}

class Geometer::Application : public wxApp
{
public:

	Application( void );
	virtual ~Application( void );

	virtual bool OnInit( void ) override;
	virtual int OnExit( void ) override;

	lua_State* LuaState( void ) { return L; }
	Frame* GetFrame( void ) { return frame; }

	bool InsertGeometry( Geometry* givenGeometry );
	bool RemoveGeometry( Geometry* givenGeometry, bool freeGeometry = true );
	Geometry* LookupGeometry( GLuint selectionID );
	void DeleteAllGeometries( void );
	const GeometryMap& GetGeometryMap( void );

private:

	Frame* frame;

	lua_State* L;

	GeometryMap geometryMap;

	// TODO: May need to own formula list here for the dependency graph.  More thought on the matter is needed.

	class ErrorHandler : public GACompute::Error::Handler
	{
	public:
		virtual void Handle( const GACompute::Error* error ) override;
	};
};

namespace Geometer
{
	wxDECLARE_APP( Application );
}

#endif __Application_h__

// Application.h