// Upgrader.cpp

#include "CornucopiaEditor.h"

using namespace CornucopiaEditor;

//=================================================================================
Upgrader::Upgrader( void )
{
}

//=================================================================================
/*virtual*/ Upgrader::~Upgrader( void )
{
}

//=================================================================================
/*virtual*/ wxDirTraverseResult Upgrader::OnDir( const wxString& dirName )
{
	return wxDIR_CONTINUE;
}

//=================================================================================
/*virtual*/ wxDirTraverseResult Upgrader::OnFile( const wxString& fileName )
{
	fileList.Add( fileName );
	return wxDIR_CONTINUE;
}

//=================================================================================
/*virtual*/ wxDirTraverseResult Upgrader::OnOpenError( const wxString& openErrorName )
{
	report += wxString::Format( "Failed to open directory \"%s\".\n", openErrorName.c_str() );
	return wxDIR_IGNORE;
}

//=================================================================================
void Upgrader::PerformUpgrade( void )
{
	wxGetApp().GetContext().SetAutoUpdateOverride( Cornucopia::Context::OVERRIDE_AUTO_UPDATE_AS_ALWAYS );

	int fileCount = fileList.GetCount();
	wxGenericProgressDialog progressDialog( wxT( "Upgrade Process In Progress" ), wxT( "Upgrading files..." ), fileCount, wxGetApp().GetFrame(), wxPD_APP_MODAL | wxPD_AUTO_HIDE | wxPD_CAN_ABORT | wxPD_REMAINING_TIME );

	for( int index = 0; index < fileCount; index++ )
	{
		wxString fileName = fileList[ index ];
		bool cancel = !progressDialog.Update( index, wxString::Format( wxT( "Upgrading file \"%s\"..." ), fileName.c_str() ) );
		if( cancel )
		{
			report += wxT( "Process aborted!\n" );
			break;
		}

		// Upgrading a file is simply a matter of reading and writing the file,
		// because the underlying Cornucopia core library will perform the upgrade.
		// This is important, because we want tools that process Cornucopia files
		// to read in upgraded files as well.
		Tree tree;
		if( !tree.Open( fileName ) )
			report += wxString::Format( "Failed to open file \"%s\".\n", fileName.c_str() );
		else if( !tree.Save() )
			report += wxString::Format( "Failed to save file \"%s\".\n", fileName.c_str() );
		else
			report += wxString::Format( "Successfully upgraded file \"%s\".\n", fileName.c_str() );
		tree.Close();
	}

	progressDialog.Update( fileCount, wxT( "Process complete!" ) );

	wxGetApp().GetContext().SetAutoUpdateOverride( Cornucopia::Context::DONT_OVERRIDE_AUTO_UPDATE );
}

//=================================================================================
const wxString& Upgrader::GetReport( void )
{
	return report;
}

// Upgrader.cpp