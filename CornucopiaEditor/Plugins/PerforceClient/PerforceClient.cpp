// PerforceClient.cpp

#include "PerforceClient.h"

// TODO: How might we handle merging?

//=================================================================================
extern "C" __declspec( dllexport ) CornucopiaEditor::Plugin* CreatePlugin( void )
{
	return new PerforceClientPlugin();
}

//=================================================================================
PerforceClientPlugin::PerforceClientPlugin( void )
{
	toolBar = 0;

	fileStatusMap = new FileStatusMap();
}

//=================================================================================
/*virtual*/ PerforceClientPlugin::~PerforceClientPlugin()
{
	delete fileStatusMap;
}

//=================================================================================
void PerforceClientPlugin::BindHandlers( void )
{
	Bind( CornucopiaEditor::CORNUCOPIA_PLUGIN_PAGE_CREATED, &PerforceClientPlugin::OnPageCreated, this );
	Bind( CornucopiaEditor::CORNUCOPIA_PLUGIN_PAGE_CHANGED, &PerforceClientPlugin::OnPageChanged, this );
	Bind( CornucopiaEditor::CORNUCOPIA_PLUGIN_PAGE_DESTROYING, &PerforceClientPlugin::OnPageDestroying, this );
	Bind( CornucopiaEditor::CORNUCOPIA_PLUGIN_FRAME_ACTIVATED, &PerforceClientPlugin::OnFrameActivated, this );
	Bind( CornucopiaEditor::CORNUCOPIA_PLUGIN_FRAME_GOT_FOCUS, &PerforceClientPlugin::OnFrameSetFocus, this );
	Bind( CornucopiaEditor::CORNUCOPIA_PLUGIN_FRAME_LOST_FOCUS, &PerforceClientPlugin::OnFrameKillFocus, this );
	Bind( CornucopiaEditor::CORNUCOPIA_PLUGIN_FILE_RESOLVE_NONWRITABLE, &PerforceClientPlugin::OnResolveNonWritableFile, this );
	Bind( CornucopiaEditor::CORNUCOPIA_PLUGIN_FILE_CREATED, &PerforceClientPlugin::OnFileCreated, this );
}

//=================================================================================
void PerforceClientPlugin::UnbindHandlers( void )
{
	Unbind( CornucopiaEditor::CORNUCOPIA_PLUGIN_PAGE_CREATED, &PerforceClientPlugin::OnPageCreated, this );
	Unbind( CornucopiaEditor::CORNUCOPIA_PLUGIN_PAGE_CHANGED, &PerforceClientPlugin::OnPageChanged, this );
	Unbind( CornucopiaEditor::CORNUCOPIA_PLUGIN_PAGE_DESTROYING, &PerforceClientPlugin::OnPageDestroying, this );
	Unbind( CornucopiaEditor::CORNUCOPIA_PLUGIN_FRAME_ACTIVATED, &PerforceClientPlugin::OnFrameActivated, this );
	Unbind( CornucopiaEditor::CORNUCOPIA_PLUGIN_FRAME_GOT_FOCUS, &PerforceClientPlugin::OnFrameSetFocus, this );
	Unbind( CornucopiaEditor::CORNUCOPIA_PLUGIN_FRAME_LOST_FOCUS, &PerforceClientPlugin::OnFrameKillFocus, this );
	Unbind( CornucopiaEditor::CORNUCOPIA_PLUGIN_FILE_RESOLVE_NONWRITABLE, &PerforceClientPlugin::OnResolveNonWritableFile, this );
	Unbind( CornucopiaEditor::CORNUCOPIA_PLUGIN_FILE_CREATED, &PerforceClientPlugin::OnFileCreated, this );
}

//=================================================================================
/*virtual*/ bool PerforceClientPlugin::Initialize( CornucopiaEditor::PluginAssistant* assitant )
{
	// It should be safe to cache this pointer away.
	this->assistant = assitant;

	wxFrame* frame = assitant->GetFrame();
	toolBar = new ToolBar( frame, this );
	toolBar->UpdateInterface();

	wxAuiManager* auiManager = assitant->GetFrameAuiManager();
	auiManager->AddPane( toolBar, wxAuiPaneInfo().ToolbarPane().Bottom() );
	auiManager->Update();

	BindHandlers();

	return true;
}

//=================================================================================
/*virtual*/ void PerforceClientPlugin::Finalize( CornucopiaEditor::PluginAssistant* assitant )
{
	wxAuiManager* auiManager = assitant->GetFrameAuiManager();
	auiManager->DetachPane( toolBar );
	auiManager->Update();

	wxFrame* frame = assitant->GetFrame();
	frame->RemoveChild( toolBar );
	toolBar->Destroy();
}

//=================================================================================
/*virtual*/ void PerforceClientPlugin::ProvideInfo( Info& pluginInfo )
{
	pluginInfo.pluginAuthor = wxT( "Spencer T. Parkin" );
	pluginInfo.pluginName = wxT( "PerforceClient" );
	pluginInfo.pluginVersion = wxT( "v1.0" );
	pluginInfo.pluginDesc = wxT( "For open files, this plugin provides an interface to perforce when those files are under source control." );
}

//=================================================================================
void PerforceClientPlugin::OnPageCreated( CornucopiaEditor::PluginPageEvent& event )
{
	wxWindow* page = event.GetPage();
	toolBar->UpdateInterface( page );
}

//=================================================================================
void PerforceClientPlugin::OnPageChanged( CornucopiaEditor::PluginPageEvent& event )
{
	wxWindow* page = event.GetPage();
	toolBar->UpdateInterface( page );
}

//=================================================================================
void PerforceClientPlugin::OnPageDestroying( CornucopiaEditor::PluginPageEvent& event )
{
	wxWindow* page = event.GetPage();
	wxString file = assistant->GetFileFromPage( page );
	if( !file.IsEmpty() )
		InvalidateFileStatus( file );
}

//=================================================================================
void PerforceClientPlugin::OnFrameActivated( CornucopiaEditor::PluginFrameEvent& event )
{
	wxFrame* frame = event.GetFrame();
	InvalidateFileStatusCache();
	if( event.GetActivated() )
		toolBar->UpdateInterface();
}

//=================================================================================
void PerforceClientPlugin::OnFrameSetFocus( CornucopiaEditor::PluginFrameEvent& event )
{
	//InvalidateFileStatusCache();
	//toolBar->UpdateInterface();
}

//=================================================================================
void PerforceClientPlugin::OnFrameKillFocus( CornucopiaEditor::PluginFrameEvent& event )
{
	//InvalidateFileStatusCache();
}

//=================================================================================
void PerforceClientPlugin::OnFileCreated( CornucopiaEditor::PluginFileEvent& event )
{
	wxString file = event.GetFile();
	toolBar->UpdateInterface( file );
}

//=================================================================================
void PerforceClientPlugin::OnResolveNonWritableFile( CornucopiaEditor::PluginFileEvent& event )
{
	wxString file = event.GetFile();
	FileStatusOperation::FileStatus* fileStatus = GetFileStatus( file );
	if( !fileStatus || fileStatus->unknown )
	{
		event.Skip();
		return;
	}
	
	if( fileStatus->openFor != FileStatusOperation::FileStatus::NOT_OPEN )
	{
		event.Skip();
		return;
	}

	if( !fileStatus->haveLatestRevision )
	{
		event.Skip();
		return;
	}

	OpenForEditOperation openForEditOperation;
	toolBar->PerformBasicOperationOnCurrentFile( &openForEditOperation );
}

//=================================================================================
FileStatusOperation::FileStatus* PerforceClientPlugin::GetFileStatus( const wxString& file )
{
	FileStatusMap::iterator iter = fileStatusMap->find( file );
	if( iter == fileStatusMap->end() )
	{
		UpdateFileStatus( file );
		iter = fileStatusMap->find( file );
		if( iter == fileStatusMap->end() )
			return 0;
	}
	
	return &iter->second;
}

//=================================================================================
void PerforceClientPlugin::UpdateFileStatus( const wxString& file )
{
	FileStatusOperation fileStatusOperation;
	fileStatusOperation.SetFile( file );
	if( fileStatusOperation.Perform() )
		( *fileStatusMap )[ file ] = fileStatusOperation.fileStatus;
	else
	{
		// If we fail to update the status of the file, then assume
		// that the failure is perminant, and so unbind our handler
		// so that we don't continually fail and annoy the user.
		// A better solution would be to disable our plugin for just
		// this file, but for now, do this as an easy fix.
		UnbindHandlers();
	}
}

//=================================================================================
void PerforceClientPlugin::InvalidateFileStatus( const wxString& file )
{
	fileStatusMap->erase( file );
}

//=================================================================================
void PerforceClientPlugin::InvalidateFileStatusCache( void )
{
	fileStatusMap->clear();
}

//=================================================================================
BEGIN_EVENT_TABLE( PerforceClientPlugin::ToolBar, wxAuiToolBar )

	EVT_BUTTON( PerforceClientPlugin::ToolBar::ID_Add, OnAdd )
	EVT_BUTTON( PerforceClientPlugin::ToolBar::ID_CheckOut, OnCheckOut )
	EVT_BUTTON( PerforceClientPlugin::ToolBar::ID_Submit, OnSubmit )
	EVT_BUTTON( PerforceClientPlugin::ToolBar::ID_Revert, OnRevert )
	EVT_BUTTON( PerforceClientPlugin::ToolBar::ID_Lock, OnLock )
	EVT_BUTTON( PerforceClientPlugin::ToolBar::ID_Unlock, OnUnlock )
	EVT_BUTTON( PerforceClientPlugin::ToolBar::ID_GetLatest, OnGetLatest )

END_EVENT_TABLE()

//=================================================================================
PerforceClientPlugin::ToolBar::ToolBar( wxWindow* parent, PerforceClientPlugin* plugin ) : wxAuiToolBar( parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_HORIZONTAL )
{
	this->plugin = plugin;

	addButton = new wxButton( this, ID_Add, wxT( "Add" ) );
	checkOutButton = new wxButton( this, ID_CheckOut, wxT( "Check Out" ) );
	submitButton = new wxButton( this, ID_Submit, wxT( "Submit" ) );
	revertButton = new wxButton( this, ID_Revert, wxT( "Revert" ) );
	lockButton = new wxButton( this, ID_Lock, wxT( "Lock" ) );
	unlockButton = new wxButton( this, ID_Unlock, wxT( "Unlock" ) );
	getLatestButton = new wxButton( this, ID_GetLatest, wxT( "Get Latest" ) );
	fileStatusText = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 1000, -1 ), wxTE_READONLY );

	AddControl( addButton );
	AddControl( checkOutButton );
	AddControl( submitButton );
	AddControl( revertButton );
	AddControl( lockButton );
	AddControl( unlockButton );
	AddControl( getLatestButton );
	AddControl( fileStatusText );

	UpdateInterface( wxEmptyString );
}

//=================================================================================
/*virtual*/ PerforceClientPlugin::ToolBar::~ToolBar( void )
{
}

//=================================================================================
void PerforceClientPlugin::ToolBar::UpdateInterface( wxWindow* page /*= 0*/ )
{
	// What file are we dealing with?
	wxString file;
	if( page )
		file = plugin->assistant->GetFileFromPage( page );
	else
		file = plugin->assistant->GetCurrentFile();

	// If we have a file, go update our interface with it.
	if( !file.IsEmpty() )
		UpdateInterface( file );
}

//=================================================================================
void PerforceClientPlugin::ToolBar::UpdateInterface( const wxString& file )
{
	// Mark all buttons as disabled.  We will enable the relavent buttons.
	// Also clear the status text.  We do not yet know the status.
	addButton->Enable( false );
	checkOutButton->Enable( false );
	submitButton->Enable( false );
	revertButton->Enable( false );
	lockButton->Enable( false );
	unlockButton->Enable( false );
	getLatestButton->Enable( false );
	fileStatusText->Clear();

	// If there's no file, everything should be disabled.
	if( file.IsEmpty() )
		return;

	// Pick out just the name of the file, since that's shorter to display.
	wxFileName fileName( file );
	wxString justName = fileName.GetName();

	// Can we get some status information on the file?
	FileStatusOperation::FileStatus* fileStatus = plugin->GetFileStatus( file );
	if( !fileStatus || fileStatus->unknown )
	{
		fileStatusText->AppendText( "The source control status of \"" + justName + "\" is unknown.  " );
		return;
	}

	// Go enable which buttons we should have available to the user based on the file status.
	// Also formulate the file status text control's message to the user.
	addButton->Enable( !fileStatus->underSourceControl );
	if( !fileStatus->underSourceControl )
		fileStatusText->AppendText( "The file \"" + justName + "\" is not under source control.  " );
	else
	{
		if( fileStatus->openFor == FileStatusOperation::FileStatus::NOT_OPEN )
		{
			fileStatusText->AppendText( "The file \"" + justName + "\" is NOT checked out by you.  " );

			// Perforce will let you open for edit a file, even if you're not at its head revision,
			// but this puts the user in a state where they may need to potentially sync/resolve,
			// which is not something that we're currently supporting in this plugin.  Hmmm.
			if( fileStatus->haveLatestRevision )
				checkOutButton->Enable( true );
		}
		else
		{
			switch( fileStatus->openFor )
			{
			case FileStatusOperation::FileStatus::OPEN_FOR_ADD: 
				fileStatusText->AppendText( "You have the file \"" + justName + "\" open for ADD.  " );
				break;
			case FileStatusOperation::FileStatus::OPEN_FOR_DELETE:
				fileStatusText->AppendText( "You have the file \"" + justName + "\" open for DELETE.  " );
				break;
			case FileStatusOperation::FileStatus::OPEN_FOR_EDIT:
				fileStatusText->AppendText( "You have the file \"" + justName + "\" open for EDIT.  " );
				break;
			}

			submitButton->Enable( true );
			revertButton->Enable( true );
			lockButton->Enable( !fileStatus->lockedByMe );
			unlockButton->Enable( fileStatus->lockedByMe );

			if( fileStatus->lockedByMe )
				fileStatusText->AppendText( "You have the file LOCKED.  " );

			if( fileStatus->lockedBySomeoneElse )
				fileStatusText->AppendText( "Someone else has the file LOCKED.  " );
		}

		if( !fileStatus->haveLatestRevision )
		{
			getLatestButton->Enable( true );
			fileStatusText->AppendText( "You do NOT have the latest revision!  " );
		}
	}
}

//=================================================================================
bool PerforceClientPlugin::ToolBar::PerformBasicOperationOnCurrentFile(
													PerforceOperation* perforceOperation,
													bool doStatusUpdate /*= true*/, bool requestReload /*= false*/ )
{
	wxString file = plugin->assistant->GetCurrentFile();
	if( file.IsEmpty() )
		return false;
	
	perforceOperation->SetFile( file );
	bool success = perforceOperation->Perform();

	if( success && requestReload )
	{
		wxFileName fileName( file );
		wxString justName = fileName.GetName();
		wxWindow* page = plugin->assistant->GetCurrentPage();
		wxString reason = "The source control system has changed the file \"" + justName + "\" on disk.";
		plugin->assistant->RequestPageReload( page, reason );
	}

	if( doStatusUpdate )
	{
		plugin->InvalidateFileStatus( file );
		UpdateInterface();
	}

	return success;
}

//=================================================================================
void PerforceClientPlugin::ToolBar::OnAdd( wxCommandEvent& event )
{
	AddOperation addOperation;
	PerformBasicOperationOnCurrentFile( &addOperation );
}

//=================================================================================
void PerforceClientPlugin::ToolBar::OnCheckOut( wxCommandEvent& event )
{
	OpenForEditOperation openForEditOperation;
	PerformBasicOperationOnCurrentFile( &openForEditOperation );
}

//=================================================================================
void PerforceClientPlugin::ToolBar::OnSubmit( wxCommandEvent& event )
{
	SubmitOperation submitOperation;
	PerformBasicOperationOnCurrentFile( &submitOperation );
}

//=================================================================================
void PerforceClientPlugin::ToolBar::OnRevert( wxCommandEvent& event )
{
	wxString file = plugin->assistant->GetCurrentFile();
	FileStatusOperation::FileStatus* fileStatus = plugin->GetFileStatus( file );
	if( fileStatus )
	{
		bool requestReload = ( fileStatus->openFor == FileStatusOperation::FileStatus::OPEN_FOR_EDIT ) ? true : false;

		RevertOperation revertOperation;
		PerformBasicOperationOnCurrentFile( &revertOperation, true, requestReload );
	}
}

//=================================================================================
void PerforceClientPlugin::ToolBar::OnLock( wxCommandEvent& event )
{
	LockOperation lockOperation;
	PerformBasicOperationOnCurrentFile( &lockOperation );
}

//=================================================================================
void PerforceClientPlugin::ToolBar::OnUnlock( wxCommandEvent& event )
{
	UnlockOperation unlockOperation;
	PerformBasicOperationOnCurrentFile( &unlockOperation );
}

//=================================================================================
void PerforceClientPlugin::ToolBar::OnGetLatest( wxCommandEvent& event )
{
	GetLatestOperation getLatestOperation;
	PerformBasicOperationOnCurrentFile( &getLatestOperation, true, true );
}

// PerforceClient.cpp