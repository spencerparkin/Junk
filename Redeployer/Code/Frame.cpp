// Frame.cpp

#include "Frame.h"
#include "TaskBarIcon.h"
#include "Server.h"
#include <wx/msgdlg.h>
#include <wx/aboutdlg.h>
#include <wx/menu.h>
#include <wx/sizer.h>
#include <wx/msw/private.h>
#include <Windows.h>
#include "../resource.h"

Frame::Frame( wxWindow* parent ) : wxFrame( parent, wxID_ANY, "Redeployer", wxDefaultPosition, wxDefaultSize, wxSYSTEM_MENU | wxRESIZE_BORDER | wxCLOSE_BOX | wxCAPTION | wxCLIP_CHILDREN | wxFRAME_NO_TASKBAR ), timer( this, ID_Timer )
{
	server = new Server();

	taskBarIcon = nullptr;

	HINSTANCE hInstance = wxGetInstance();
	HICON hIcon = LoadIcon( hInstance, MAKEINTRESOURCE( IDI_ICON1 ) );

	wxIcon icon;
	if( !icon.CreateFromHICON( hIcon ) )
		wxMessageBox( "Failed to load icon.", "Error", wxICON_ERROR | wxCENTRE );
	else
	{
		taskBarIcon = new TaskBarIcon();
		taskBarIcon->SetIcon( icon );
	}

	wxMenu* programMenu = new wxMenu();
	wxMenuItem* hideMenuItem = new wxMenuItem( programMenu, ID_Hide, "Hide" );
	wxMenuItem* exitMenuItem = new wxMenuItem( programMenu, ID_Exit, "Exit" );
	programMenu->Append( hideMenuItem );
	programMenu->AppendSeparator();
	programMenu->Append( exitMenuItem );

	wxMenu* helpMenu = new wxMenu();
	wxMenuItem* aboutMenuItem = new wxMenuItem( helpMenu, ID_About, "About" );
	helpMenu->Append( aboutMenuItem );

	wxMenuBar* menuBar = new wxMenuBar();
	menuBar->Append( programMenu, "Program" );
	menuBar->Append( helpMenu, "Help" );
	SetMenuBar( menuBar );

	textCtrl = new wxTextCtrl( this, wxID_ANY );

	wxBoxSizer* boxSizer = new wxBoxSizer( wxVERTICAL );
	boxSizer->Add( textCtrl, 1, wxGROW );
	SetSizer( boxSizer );

	Bind( wxEVT_MENU, &Frame::OnExit, this, ID_Exit );
	Bind( wxEVT_MENU, &Frame::OnHide, this, ID_Hide );
	Bind( wxEVT_MENU, &Frame::OnAbout, this, ID_About );
	Bind( wxEVT_TIMER, &Frame::OnTimer, this, ID_Timer );

	timer.Start( 100 );
}

/*virtual*/ Frame::~Frame( void )
{
	delete taskBarIcon;
	delete server;
}

void Frame::AddLogMessage( const wxString& logMessage )
{
	// TODO: Prefix time-stamp.
	textCtrl->AppendText( logMessage + "\n" );

	// TODO: Also flush log message to disk.
}

void Frame::OnTimer( wxTimerEvent& event )
{
	if( !server->Run() )
	{
		wxMessageBox( "Server failed to run.", "Error", wxICON_ERROR | wxCENTRE, this );
		timer.Stop();
	}
}

void Frame::OnHide( wxCommandEvent& event )
{
	Show( false );
}

void Frame::OnExit( wxCommandEvent& event )
{
	Close( true );
}

void Frame::OnAbout( wxCommandEvent& event )
{
	wxAboutDialogInfo aboutDialogInfo;

	aboutDialogInfo.SetName( wxT( "Spawn Redeployer" ) );
	aboutDialogInfo.SetVersion( wxT( "1.0" ) );
	aboutDialogInfo.SetDescription( wxT( "Redeploy spawn-related server and user applications." ) );
	aboutDialogInfo.SetCopyright( wxT( "Copyright (C) 2017, 3M HIS, All Rights Reserved" ) );

	wxAboutBox( aboutDialogInfo );
}

// Frame.cpp