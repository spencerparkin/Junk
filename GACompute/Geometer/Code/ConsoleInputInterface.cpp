// ConsoleInputInterface.cpp

#include "ConsoleInputInterface.h"
#include "Application.h"
#include "LuaIdioms.h"

#include <wx/msgdlg.h>
#include <wx/button.h>
#include <wx/file.h>

#include <lua.hpp>
#include <lauxlib.h>
#include <lualib.h>

namespace Geometer
{
	wxIMPLEMENT_DYNAMIC_CLASS( ConsoleInputInterface, Interface )
}

using namespace Geometer;

ConsoleInputInterface::ConsoleInputInterface( void )
{
	historyIndex = -1;
}

/*virtual*/ ConsoleInputInterface::~ConsoleInputInterface( void )
{
}

/*virtual*/ void ConsoleInputInterface::FillOutRegistryEntry( RegistryEntry& entry )
{
	entry.interfaceMenuItemName = "Console Input";
	entry.interfaceMenuItemHelp = "Toggle the conole input interface.";
}

/*virtual*/ bool ConsoleInputInterface::CreateControls( void )
{
	textControl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_PROCESS_ENTER );

	wxButton* executeButton = new wxButton( this, wxID_ANY, "Execute" );

	wxBoxSizer* boxSizer = new wxBoxSizer( wxVERTICAL );
	boxSizer->Add( textControl, 1, wxGROW );
	boxSizer->Add( executeButton, 0, wxGROW );
	SetSizer( boxSizer );

	// A button should not be a bad way to go, because the user should
	// be able to press TAB-ENTER.  The button will then give the focus
	// back to the text control.
	Bind( wxEVT_BUTTON, &ConsoleInputInterface::OnExecute, this );

	Bind( wxEVT_CHAR_HOOK, &ConsoleInputInterface::OnCharHook, this );

	FileDropTarget* fileDropTarget = new FileDropTarget();
	fileDropTarget->textControl = textControl;
	textControl->SetDropTarget( fileDropTarget );

	return true;
}

void ConsoleInputInterface::OnCharHook( wxKeyEvent& event )
{
	if( event.ControlDown() )
	{
		int historyIndexDelta = 0;

		switch( event.GetKeyCode() )
		{
			case WXK_UP:
			{
				historyIndexDelta = -1;
				break;
			}
			case WXK_DOWN:
			{
				historyIndexDelta = 1;
				break;
			}
		}

		if( historyIndexDelta != 0 )
		{
			lua_State* L = wxGetApp().LuaState();
			LuaStackPopper stackPopper(L);

			lua_getglobal( L, "console" );
			if( lua_isnil( L, -1 ) )
				return;

			lua_getfield( L, -1, "input" );
			if( lua_isnil( L, -1 ) )
				return;

			int length = lua_rawlen( L, -1 );

			if( historyIndex == -1 )
				historyIndex = ( event.GetKeyCode() == WXK_UP ) ? length : 1;
			else
			{
				historyIndex += historyIndexDelta;
				if( historyIndex > length )
					historyIndex = 1;
				else if( historyIndex < 1 )
					historyIndex = length;
			}

			lua_pushinteger( L, historyIndex );
			lua_gettable( L, -2 );
			wxString text = lua_tostring( L, -1 );
			if( text.IsEmpty() )
				return;

			textControl->SetValue( text );
			return;
		}
	}

	event.Skip();
}

/*virtual*/ bool ConsoleInputInterface::UpdateControls( void )
{
	return true;
}

void ConsoleInputInterface::OnExecute( wxCommandEvent& event )
{
	// TODO: Start wait cursor?

	lua_State* L = wxGetApp().LuaState();
	LuaStackPopper stackPopper(L);

	// TODO: Why is the text clearing after I enter stuff that's syntactically wrong?  I'd like a chance to edit what I had and try again.

	wxString text = textControl->GetValue();
	int result = luaL_loadbuffer( L, text, text.Length(), "consoleInputChunk" );
	if( result == LUA_ERRSYNTAX )
	{
		wxString error = lua_tostring( L, -1 );
		wxString message = "Geometer console input: " + error;
		wxMessageDialog messageDialog( wxGetApp().GetFrame(), message, "Lua Syntax Error", wxOK | wxCENTRE | wxICON_ERROR );
		messageDialog.ShowModal();
		return;
	}

	textControl->Clear();

	result = lua_pcall( L, 0, 0, 0 );
	if( result == LUA_ERRRUN )
	{
		wxString error = lua_tostring( L, -1 );
		wxString message = "Geometer console input: " + error;
		wxMessageDialog messageDialog( wxGetApp().GetFrame(), message, "Lua Run-time Error", wxOK | wxCENTRE | wxICON_ERROR );
		messageDialog.ShowModal();
		return;
	}

	textControl->SetFocus();

	if( l_getornewtable( L, "console", true ) )
	{
		l_getornewtable( L, "input" );
		int length = lua_rawlen( L, -1 );
		lua_pushinteger( L, length + 1 );
		lua_pushstring( L, text.c_str() );
		lua_settable( L, -3 );
	}

	historyIndex = -1;

	TouchedLuaState( false );
}

ConsoleInputInterface::FileDropTarget::FileDropTarget( void )
{
	textControl = 0;
}

/*virtual*/ ConsoleInputInterface::FileDropTarget::~FileDropTarget( void )
{
}

/*virtual*/ bool ConsoleInputInterface::FileDropTarget::OnDropFiles( wxCoord x, wxCoord y, const wxArrayString& fileNames )
{
	if( !textControl )
		return false;

	for( unsigned int i = 0; i < fileNames.GetCount(); i++ )
	{
		wxString filePath = fileNames[i];
		wxFile file( filePath, wxFile::read );
		wxString text;
		if( !file.ReadAll( &text ) )
			continue;

		textControl->AppendText( "-- " + filePath + "\n\n" );
		textControl->AppendText( text );
		textControl->AppendText( "\n\n" );
	}

	return true;
}

// ConsoleInputInterface.cpp