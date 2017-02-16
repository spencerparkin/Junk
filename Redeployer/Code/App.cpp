// App.cpp

#include "App.h"
#include "Frame.h"
#include <wx/msgdlg.h>
#include <wx/taskbar.h>

wxIMPLEMENT_APP( App );

App::App( void )
{
	frame = nullptr;
}

/*virtual*/ App::~App( void )
{
}

/*virtual*/ bool App::OnInit( void )
{
	if( !wxApp::OnInit() )
		return false;

	if( !wxTaskBarIcon::IsAvailable() )
		return false;

	frame = new Frame( nullptr );
	SetTopWindow( frame );

	return true;
}

// App.cpp