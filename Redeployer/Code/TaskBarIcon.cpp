// TaskBarIcon.cpp

#include "TaskBarIcon.h"
#include "Frame.h"
#include "App.h"

TaskBarIcon::TaskBarIcon( void )
{
}

/*virtual*/ TaskBarIcon::~TaskBarIcon( void )
{
}

/*virtual*/ wxMenu* TaskBarIcon::CreatePopupMenu( void )
{
	wxMenu* taskBarMenu = new wxMenu();
	wxMenuItem* toggleFrameMenuItem = new wxMenuItem( taskBarMenu, ID_ToggleFrame, "Toggle Window", "", wxITEM_CHECK );
	wxMenuItem* exitMenuItem = new wxMenuItem( taskBarMenu, ID_Exit, "Exit" );
	taskBarMenu->Append( toggleFrameMenuItem );
	taskBarMenu->AppendSeparator();
	taskBarMenu->Append( exitMenuItem );

	Bind( wxEVT_MENU, &TaskBarIcon::OnExit, this, ID_Exit );
	Bind( wxEVT_MENU, &TaskBarIcon::OnToggleFrame, this, ID_ToggleFrame );
	Bind( wxEVT_UPDATE_UI, &TaskBarIcon::OnUpdateUI, this, ID_ToggleFrame );

	return taskBarMenu;
}

void TaskBarIcon::OnExit( wxCommandEvent& event )
{
	RemoveIcon();

	wxGetApp().GetFrame()->Close( true );
}

void TaskBarIcon::OnToggleFrame( wxCommandEvent& event )
{
	wxFrame* frame = wxGetApp().GetFrame();
	frame->Show( !frame->IsShown() );
}

void TaskBarIcon::OnUpdateUI( wxUpdateUIEvent& event )
{
	switch( event.GetId() )
	{
		case ID_ToggleFrame:
		{
			event.Check( wxGetApp().GetFrame()->IsShown() );
			break;
		}
	}
}

// TaskBarIcon.cpp