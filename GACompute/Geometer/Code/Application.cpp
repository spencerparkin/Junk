// Application.cpp

#include "Application.h"
#include "Interface.h"
#include "Function.h"
#include "Geometry.h"
#include <wx/msgdlg.h>

namespace Geometer
{
	wxIMPLEMENT_APP( Application );
}

using namespace Geometer;

Application::Application( void )
{
	frame = 0;

	L = 0;
}

/*virtual*/ Application::~Application( void )
{
	DeleteAllGeometries();
}

/*virtual*/ bool Application::OnInit( void )
{
	if( !wxApp::OnInit() )
		return false;

	GACompute::Error::SetHandler( new ErrorHandler() );

	wxImage::AddHandler( new wxPNGHandler );

	Interface::PopulateRegistry();

	L = luaL_newstate();
	if( !L )
		return false;

	luaL_openlibs(L);
	
	Function::RegisterFunctions();

	frame = new Frame(0);
	frame->Show( true );

	return true;
}

/*virtual*/ int Application::OnExit( void )
{
	lua_close(L);

	GACompute::Error::SetHandler(0);

	return wxApp::OnExit();
}

/*virtual*/ void Application::ErrorHandler::Handle( const GACompute::Error* error )
{
	wxString errorMessage = error->GetString();
	wxMessageBox( errorMessage, "GACompute Error", wxCENTRE | wxOK | wxICON_ERROR, wxGetApp().GetFrame() );
}

const GeometryMap& Application::GetGeometryMap( void )
{
	return geometryMap;
}

bool Application::InsertGeometry( Geometry* givenGeometry )
{
	GLuint selectionID = givenGeometry->SelectionID();
	Geometry* geometry = LookupGeometry( selectionID );
	if( geometry )
		return( geometry == givenGeometry ? true : false );

	geometryMap[ selectionID ] = geometry;
	return true;
}

bool Application::RemoveGeometry( Geometry* givenGeometry, bool freeGeometry /*= true*/ )
{
	GLuint selectionID = givenGeometry->SelectionID();
	Geometry* geometry = LookupGeometry( selectionID );
	if( !geometry || geometry != givenGeometry )
		return false;

	if( freeGeometry )
		delete geometry;

	geometryMap[ selectionID ] = 0;
	return true;
}

Geometry* Application::LookupGeometry( GLuint selectionID )
{
	GeometryMap::iterator iter = geometryMap.find( selectionID );
	if( iter == geometryMap.end() )
		return 0;

	Geometry* geometry = iter->second;
	return geometry;
}

void Application::DeleteAllGeometries( void )
{
	while( geometryMap.size() > 0 )
	{
		GeometryMap::iterator iter = geometryMap.begin();
		Geometry* geometry = iter->second;
		delete geometry;
		geometryMap.erase( iter );
	}
}

// Application.cpp