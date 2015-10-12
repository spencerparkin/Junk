// ConsoleOutputInterface.cpp

#include "ConsoleOutputInterface.h"
#include "Application.h"
#include "LuaIdioms.h"

#include <wx/file.h>

#include <lua.hpp>
#include <lauxlib.h>
#include <lualib.h>

namespace Geometer
{
	wxIMPLEMENT_DYNAMIC_CLASS( ConsoleOutputInterface, Interface )
}

using namespace Geometer;

ConsoleOutputInterface::ConsoleOutputInterface( void )
{
	textControl = 0;

	entriesRead = 0;

	// TODO: Don't hard-code these anymore.
#if 1
	latexBinDirPath = "C:\\Program Files\\MiKTeX 2.9\\miktex\\bin";
	latexWorkDirPath = "D:\\GACompute\\Latex";
#else
	latexBinDirPath = "C:\\Program Files (x86)\\MiKTeX 2.9\\miktex\\bin";
	latexWorkDirPath = "C:\\GACompute\\Latex";
#endif
}

/*virtual*/ ConsoleOutputInterface::~ConsoleOutputInterface( void )
{
}

/*virtual*/ void ConsoleOutputInterface::FillOutRegistryEntry( RegistryEntry& entry )
{
	entry.interfaceMenuItemName = "Console Output";
	entry.interfaceMenuItemHelp = "Toggle the conole output interface.";
}

/*virtual*/ bool ConsoleOutputInterface::CreateControls( void )
{
	textControl = new wxRichTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxRE_MULTILINE | wxRE_READONLY );

	wxBoxSizer* boxSizer = new wxBoxSizer( wxVERTICAL );
	boxSizer->Add( textControl, 1, wxGROW );
	SetSizer( boxSizer );

	return true;
}

/*virtual*/ bool ConsoleOutputInterface::UpdateControls( void )
{
	lua_State* L = wxGetApp().LuaState();
	LuaStackPopper stackPopper(L);

	lua_getglobal( L, "console" );
	if( !lua_istable( L, -1 ) )
		return false;
	
	lua_getfield( L, -1, "output" );
	if( !lua_istable( L, -1 ) )
		return false;

	lua_getfield( L, -1, "clear" );
	if( lua_isnil( L, -1 ) )
		lua_pop( L, 1 );
	else
	{
		lua_pop( L, 1 );
		lua_pushnil( L );
		lua_setfield( L, -2, "clear" );

		textControl->Clear();
		entriesRead = 0;
	}
		
	int length = lua_rawlen( L, -1 );
	if( entriesRead < length )
	{
		entriesRead++;
		lua_pushinteger( L, entriesRead );
		lua_gettable( L, -2 );
		const char* entry = lua_tostring( L, -1 );
		if( entry )
			Print( entry );
		return false;
	}

	return true;
}

void ConsoleOutputInterface::Print( const wxString& text )
{
	std::string string = text;
	std::string latexOpen = "latex{";
	std::string latexClose = "}latex";

	while( string.length() > 0 )
	{
		int openPos = string.find( latexOpen );
		if( openPos == 0 )
		{
			string.replace( 0, latexOpen.length(), "" );

			int closePos = string.find( latexClose );
			if( closePos < 0 )
			{
				textControl->AppendText( "{Open latex encountered without matching close!}" );
				return;
			}

			std::string subString = string.substr( 0, closePos );
			wxImage latexImage;
			if( GenerateLatex( subString, latexImage ) )
				textControl->WriteImage( latexImage );
			else
				textControl->AppendText( "{LaTeX Error!}" );

			string.replace( 0, closePos, "" );
			string.replace( 0, latexOpen.length(), "" );
		}
		else if( openPos > 0 )
		{
			std::string subString = string.substr( 0, openPos );
			textControl->AppendText( subString.c_str() );
			string.replace( 0, openPos, "" );
		}
		else
		{
			textControl->AppendText( string.c_str() );
			string = "";
		}
	}
}

// TODO: We may want to run this in a thread and kill it if it's going too long.
//       It might be that the latex command is waiting for input it will never get.
//       Or, don't use wxEXEC_SYNC?  Is there a wxEXEC_ASYNC?
bool ConsoleOutputInterface::GenerateLatex( const wxString& latexCode, wxImage& latexImage )
{
	wxString preLatexCode =
		"\\documentclass[fleqn]{article}\n"
		"\n"
		"\\usepackage{amsmath}\n"
		"\\usepackage{amssymb}\n"
		"\\usepackage{amsfonts}\n"
		"\\usepackage[margin=0.05in]{geometry}\n"
		"\n"
		"\\pagestyle{empty}\n"
		"\n"
		"\\begin{document}\n"
		"\n"
		"\\begin{flalign*}\n";

	wxString postLatexCode =
		"\\end{flalign*}\n"
		"\n"
		"\\end{document}\n";

	wxString completeLatexCode = preLatexCode + latexCode + postLatexCode;

	if( !wxSetWorkingDirectory( latexWorkDirPath ) )
		return false;

	wxString latexImageFilePath = latexWorkDirPath + "\\latex1.png";
	if( wxFileExists( latexImageFilePath ) && !wxRemoveFile( latexImageFilePath ) )
		return false;

	wxString latexCodeFilePath = latexWorkDirPath + "\\latex.tex";
	wxFile latexCodeFile( latexCodeFilePath, wxFile::write );
	if( !latexCodeFile.Write( completeLatexCode ) )
		return false;

	latexCodeFile.Close();

	wxString commandLine = latexBinDirPath + "\\latex " + latexCodeFilePath;
	long exitCode = wxExecute( commandLine, wxEXEC_SYNC | wxEXEC_HIDE_CONSOLE );
	if( exitCode != 0 )
		return false;

	// TODO: We really need to do this asynchronously or in a thread so that we can cancel if the process goes too long.
	wxString latexDviFilePath = latexWorkDirPath + "\\latex.dvi";
	commandLine = latexBinDirPath + "\\dvipng " + latexDviFilePath;
	exitCode = wxExecute( commandLine, wxEXEC_SYNC | wxEXEC_HIDE_CONSOLE );
	if( exitCode != 0 )
		return false;

	if( !wxFileExists( latexImageFilePath ) )
		return false;

	if( !latexImage.LoadFile( latexImageFilePath ) )
		return false;

	return true;
}

// ConsoleOutputInterface.cpp