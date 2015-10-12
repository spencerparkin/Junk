// Frame.cpp

#include "CornucopiaEditor.h"

using namespace CornucopiaEditor;

wxIMPLEMENT_DYNAMIC_CLASS( Frame, wxFrame )

//=================================================================================
BEGIN_EVENT_TABLE( Frame, wxFrame )

	EVT_MENU( Frame::ID_NewTree, OnNewTree )
	EVT_MENU( Frame::ID_NewClone, OnNewClone )
	EVT_MENU( Frame::ID_OpenTree, OnOpenTree )
	EVT_MENU( Frame::ID_SaveTree, OnSaveTree )
	EVT_MENU( Frame::ID_UpgradeTrees, OnUpgradeTrees )
	EVT_MENU( Frame::ID_Exit, OnExit )
	EVT_MENU( Frame::ID_Help, OnHelp )
	EVT_MENU( Frame::ID_About, OnAbout )
	EVT_MENU( Frame::ID_Undo, OnUndo )
	EVT_MENU( Frame::ID_Redo, OnRedo )
	EVT_MENU( Frame::ID_Search, OnSearch )
	EVT_MENU( Frame::ID_WipeTreeCache, OnWipeTreeCache )
	EVT_MENU( Frame::ID_TouchTree, OnTouchTree )
	EVT_MENU( Frame::ID_PluginManager, OnPluginManager )
	EVT_MENU( Frame::ID_CheckUniqueness, OnCheckUniqueness )

	EVT_TEXT_ENTER( Frame::ID_SearchBox, OnSearchBoxEnter )

	EVT_BUTTON( Frame::ID_ContextSensativeHelp, OnContextSensativeHelp )

	EVT_IDLE( Frame::OnIdle )

	EVT_CLOSE( Frame::OnClose )

	EVT_SET_FOCUS( Frame::OnSetFocus )
	EVT_KILL_FOCUS( Frame::OnKillFocus )
	EVT_ACTIVATE( Frame::OnActivate )

	//EVT_FSWATCHER( Frame::OnFileWatchEvent )

END_EVENT_TABLE()

//=================================================================================
// This constructor is private and should never be used.
Frame::Frame( void )
{
}

//=================================================================================
Frame::Frame( const wxPoint& pos, const wxSize& size ) : wxFrame( ( wxFrame* )NULL, wxID_ANY, "Cornucopia Editor", pos, size )
{
	fileToolBar = 0;
	undoRedoToolBar = 0;
	searchToolBar = 0;
	helpToolBar = 0;
	searchBox = 0;
	menuBar = 0;
	statusBar = 0;
	noteBook = 0;
	contextSensativeHelpButton = 0;
	helpPanel = 0;
	pluginPanel = 0;

	auiManager.SetManagedWindow( this );

	BuildUserInterface();
}

//=================================================================================
/*virtual*/ Frame::~Frame( void )
{
	auiManager.UnInit();
}

//=================================================================================
void Frame::BuildUserInterface( void )
{
	wxBitmap bitmap;
	if( wxGetApp().GetResourceManager()->LoadBitmap( bitmap, wxT( "Art/cornucopia.bmp" ), wxBITMAP_TYPE_BMP ) )
	{
		icon.CopyFromBitmap( bitmap );
		SetIcon( icon );
	}

	SetDropTarget( new FileDropTarget( this ) );

	wxString newTreeDesc = wxT( "Create a new Cornucopia tree." );
	wxString newCloneDesc = wxT( "Create a new Cornucopia tree that is a clone of an existing tree." );
	wxString openTreeDesc = wxT( "Open an existing Cornucopia tree on disk." );
	wxString saveTreeDesc = wxT( "Save the current Cornucopia tree in focus." );

	wxMenu* fileMenu = new wxMenu();
	wxMenuItem* menuItemNewTree = new wxMenuItem( fileMenu, ID_NewTree, wxT( "&New Tree\tCtrl-N" ), newTreeDesc );
	wxMenuItem* menuItemNewClone = new wxMenuItem( fileMenu, ID_NewClone, wxT( "&New Clone\tAlt-N" ), newCloneDesc );
	wxMenuItem* menuItemOpenTree = new wxMenuItem( fileMenu, ID_OpenTree, wxT( "&Open Tree\tCtrl-O" ), openTreeDesc );
	wxMenuItem* menuItemSaveTree = new wxMenuItem( fileMenu, ID_SaveTree, wxT( "&Save Tree\tCtrl-S" ), saveTreeDesc );
	wxMenuItem* menuItemExit = new wxMenuItem( fileMenu, ID_Exit, wxT( "Exit" ), wxT( "Exit the application." ) );
	menuItemNewTree->SetBitmap( wxArtProvider::GetBitmap( wxART_NEW ) );
	menuItemNewClone->SetBitmap( wxArtProvider::GetBitmap( wxART_COPY ) );
	menuItemOpenTree->SetBitmap( wxArtProvider::GetBitmap( wxART_FILE_OPEN ) );
	menuItemSaveTree->SetBitmap( wxArtProvider::GetBitmap( wxART_FILE_SAVE ) );
	menuItemExit->SetBitmap( wxArtProvider::GetBitmap( wxART_QUIT ) );
	fileMenu->Append( menuItemNewTree );
	fileMenu->Append( menuItemNewClone );
	fileMenu->Append( menuItemOpenTree );
	fileMenu->Append( menuItemSaveTree );
	fileMenu->AppendSeparator();
	fileMenu->Append( menuItemExit );

	wxMenu* optionsMenu = new wxMenu();
	wxMenuItem* menuItemWipeTreeCache = new wxMenuItem( optionsMenu, ID_WipeTreeCache, wxT( "Wipe Tree Cache" ), wxT( "Wipes the tree cache." ) );
	wxMenuItem* menuItemTouchTree = new wxMenuItem( optionsMenu, ID_TouchTree, wxT( "Touch Tree" ), wxT( "Marks the current tree as modified." ) );
	wxMenuItem* menuItemUpgradeTrees = new wxMenuItem( optionsMenu, ID_UpgradeTrees, wxT( "Upgrade Trees\tF5" ), wxT( "Upgrade one or more trees in a given folder recursively." ) );
	wxMenuItem* menuItemPluginManager = new wxMenuItem( optionsMenu, ID_PluginManager, wxT( "Plugin Manager\tF6" ), wxT( "Bring up the plugin manager panel." ) );
	wxMenuItem* menuItemCheckUniqueness = new wxMenuItem( optionsMenu, ID_CheckUniqueness, wxT( "Check Uniqueness\tF7" ), wxT( "Find all instances of nodes that cannot be uniquely identified within the tree." ) );
	menuItemUpgradeTrees->SetBitmap( wxArtProvider::GetBitmap( wxART_FIND_AND_REPLACE ) );
	optionsMenu->Append( menuItemWipeTreeCache );
	optionsMenu->Append( menuItemTouchTree );
	optionsMenu->AppendSeparator();
	optionsMenu->Append( menuItemUpgradeTrees );
	optionsMenu->Append( menuItemPluginManager );
	optionsMenu->Append( menuItemCheckUniqueness );
	// TODO: Add option to set/get the file reference node's base path.

	wxMenu* helpMenu = new wxMenu();
	wxMenuItem* menuItemHelp = new wxMenuItem( helpMenu, ID_Help, wxT( "Documentation\tF1" ), wxT( "Open the Cornucopia documentation." ) );
	menuItemHelp->SetBitmap( wxArtProvider::GetBitmap( wxART_HELP ) );
	helpMenu->Append( menuItemHelp );
	helpMenu->AppendSeparator();
	helpMenu->Append( ID_About, wxT( "About" ), wxT( "Pop open the about dialog box." ) );

	menuBar = new wxMenuBar();
	menuBar->Append( fileMenu, wxT( "File" ) );
	menuBar->Append( optionsMenu, wxT( "Options" ) );
	menuBar->Append( helpMenu, wxT( "Help" ) );
	SetMenuBar( menuBar );

	fileToolBar = new wxAuiToolBar( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_DEFAULT_STYLE );
	fileToolBar->AddTool( ID_NewTree, wxT( "New Tree" ), wxArtProvider::GetBitmap( wxART_NEW ) );
	fileToolBar->AddTool( ID_NewClone, wxT( "New Clone" ), wxArtProvider::GetBitmap( wxART_COPY ) );
	fileToolBar->AddTool( ID_OpenTree, wxT( "Open Tree" ), wxArtProvider::GetBitmap( wxART_FILE_OPEN ) );
	fileToolBar->AddTool( ID_SaveTree, wxT( "Save Tree" ), wxArtProvider::GetBitmap( wxART_FILE_SAVE ) );
	fileToolBar->SetToolShortHelp( ID_NewTree, newTreeDesc );
	fileToolBar->SetToolShortHelp( ID_NewClone, newCloneDesc );
	fileToolBar->SetToolShortHelp( ID_OpenTree, openTreeDesc );
	fileToolBar->SetToolShortHelp( ID_SaveTree, saveTreeDesc );
	fileToolBar->Realize();
	auiManager.AddPane( fileToolBar, wxAuiPaneInfo().Name( wxT( "FileToolbar" ) ).Caption( wxT( "File Toolbar" ) ).ToolbarPane().Top() );

	undoRedoToolBar = new wxAuiToolBar( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_DEFAULT_STYLE );
	undoRedoToolBar->AddTool( ID_Undo, wxT( "Undo" ), wxArtProvider::GetBitmap( wxART_UNDO ) );
	undoRedoToolBar->AddTool( ID_Redo, wxT( "Redo" ), wxArtProvider::GetBitmap( wxART_REDO ) );
	undoRedoToolBar->SetToolShortHelp( ID_Undo, wxT( "Undo your previous alteration to the tree, if any." ) );
	undoRedoToolBar->SetToolShortHelp( ID_Redo, wxT( "Redo your next alteration to the tree, if any." ) );
	undoRedoToolBar->Realize();
	auiManager.AddPane( undoRedoToolBar, wxAuiPaneInfo().Name( wxT( "UndoRedoToolbar" ) ).Caption( wxT( "Undo-Redo Toolbar" ) ).ToolbarPane().Top() );

	searchToolBar = new wxAuiToolBar( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_DEFAULT_STYLE );
	searchToolBar->AddTool( ID_Search, wxT( "Search" ), wxArtProvider::GetBitmap( wxART_FIND ) );
	searchBox = new wxComboBox( searchToolBar, ID_SearchBox, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, 0, wxTE_PROCESS_ENTER | wxCB_DROPDOWN | wxCB_SORT );
	searchToolBar->AddControl( searchBox );
	searchToolBar->SetToolShortHelp( ID_Search, wxT( "Search for the next node by the given name." ) );
	searchToolBar->Realize();
	auiManager.AddPane( searchToolBar, wxAuiPaneInfo().Name( wxT( "SearchToolbar" ) ).Caption( wxT( "Search" ) ).ToolbarPane().Top() );

	// TODO: This is silly.  Just add it to the help menu or something like that.
	/*
	helpToolBar = new wxAuiToolBar( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_DEFAULT_STYLE );
	//helpToolBar->AddTool( ID_Help, wxT( "Documentation" ), wxArtProvider::GetBitmap( wxART_TIP ) );
	contextSensativeHelpButton = new wxButton( helpToolBar, ID_ContextSensativeHelp, wxT( "Context Sensative Help" ), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	helpToolBar->AddControl( contextSensativeHelpButton );
	helpToolBar->Realize();
	auiManager.AddPane( helpToolBar, wxAuiPaneInfo().Name( wxT( "Help" ) ).Caption( wxT( "Help" ) ).ToolbarPane().Top() );
	*/

	wxPanel* controlPanel = new wxPanel( this );
	noteBook = new NoteBook( controlPanel );

	wxBoxSizer* panelSizer = new wxBoxSizer( wxVERTICAL );
	panelSizer->Add( noteBook, 1, wxGROW | wxALL );
	controlPanel->SetSizer( panelSizer );

	wxBoxSizer* frameSizer = new wxBoxSizer( wxVERTICAL );
	frameSizer->Add( controlPanel, 1, wxGROW | wxALL );
	SetSizer( frameSizer );

	auiManager.AddPane( controlPanel, wxAuiPaneInfo().Name( wxT( "MainControlPanel" ) ).Caption( wxT( "Main Control Panel" ) ).CenterPane().PaneBorder( true ) );

	statusBar = new wxStatusBar( this );
	statusBar->PushStatusText( wxT( "" ) );
	SetStatusBar( statusBar );

	UpdateUndoRedoToolBar();
	UpdateFileToolBar();
	UpdateMenuBar();
	UpdateSearchBar();

	auiManager.Update();

	wxAcceleratorEntry acceleratorEntries[11];
	acceleratorEntries[0].Set( wxACCEL_NORMAL, WXK_F1, ID_Help );
	acceleratorEntries[1].Set( wxACCEL_NORMAL, WXK_F3, ID_Search );
	acceleratorEntries[2].Set( wxACCEL_NORMAL, WXK_F5, ID_UpgradeTrees );
	acceleratorEntries[3].Set( wxACCEL_NORMAL, WXK_F6, ID_PluginManager );
	acceleratorEntries[4].Set( wxACCEL_NORMAL, WXK_F7, ID_CheckUniqueness );
	acceleratorEntries[5].Set( wxACCEL_CTRL, int( 'n' ), ID_NewTree );
	acceleratorEntries[6].Set( wxACCEL_ALT, int( 'n' ), ID_NewClone );
	acceleratorEntries[7].Set( wxACCEL_CTRL, int( 'o' ), ID_OpenTree );
	acceleratorEntries[8].Set( wxACCEL_CTRL, int( 's' ), ID_SaveTree );
	acceleratorEntries[9].Set( wxACCEL_CTRL, int( 'z' ), ID_Undo );
	acceleratorEntries[10].Set( wxACCEL_CTRL, int( 'y' ), ID_Redo );

	// TODO: Program F12 to fix anything that might be wrong with the windows in the app, such as the tool-bars being out in outer-space.

	wxAcceleratorTable acceleratorTable( sizeof( acceleratorEntries ) / sizeof( wxAcceleratorEntry ), acceleratorEntries );
	SetAcceleratorTable( acceleratorTable );
}

//=================================================================================
void Frame::OnContextSensativeHelp( wxCommandEvent& event )
{
	wxContextHelp contextHelp( this );
}

//=================================================================================
void Frame::OnSetFocus( wxFocusEvent& event )
{
	PluginManager* pluginManager = wxGetApp().GetPluginManager();
	if( pluginManager )
	{
		PluginFrameEvent event;
		event.SetEventType( CORNUCOPIA_PLUGIN_FRAME_GOT_FOCUS );
		event.SetEventObject( this );
		event.SetFrame( this );
		pluginManager->CallAllPlugins( event );
	}

	event.Skip();
}

//=================================================================================
void Frame::OnKillFocus( wxFocusEvent& event )
{
	PluginManager* pluginManager = wxGetApp().GetPluginManager();
	if( pluginManager )
	{
		PluginFrameEvent event;
		event.SetEventType( CORNUCOPIA_PLUGIN_FRAME_LOST_FOCUS );
		event.SetEventObject( this );
		event.SetFrame( this );
		pluginManager->CallAllPlugins( event );
	}

	event.Skip();
}

//=================================================================================
void Frame::OnActivate( wxActivateEvent& event )
{
	PluginManager* pluginManager = wxGetApp().GetPluginManager();
	if( pluginManager )
	{
		PluginFrameEvent frameEvent;
		frameEvent.SetEventType( CORNUCOPIA_PLUGIN_FRAME_ACTIVATED );
		frameEvent.SetEventObject( this );
		frameEvent.SetFrame( this );
		frameEvent.SetActivated( event.GetActive() );
		pluginManager->CallAllPlugins( frameEvent );
	}
}

//=================================================================================
NoteBook* Frame::GetNoteBook( void )
{
	return noteBook;
}

//=================================================================================
wxAuiManager* Frame::GetAuiManager( void )
{
	return &auiManager;
}

//=================================================================================
void Frame::HandleDroppedFiles( const wxArrayString& fileNames )
{
	int fileCount = fileNames.GetCount();
	for( int fileIndex = 0; fileIndex < fileCount; fileIndex++ )
	{
		wxString filePath = fileNames[ fileIndex ];
		( void )OpenTree( filePath );
	}
}

//=================================================================================
void Frame::OnIdle( wxIdleEvent& event )
{
	int pageCount = noteBook->GetPageCount();
	for( int pageIndex = 0; pageIndex < pageCount; pageIndex++ )
	{
		Panel* panel = wxDynamicCast( noteBook->GetPage( pageIndex ), Panel );
		if( panel )
			panel->DoIdleProcessing();
	}

	PluginManager* pluginManager = wxGetApp().GetPluginManager();
	if( pluginManager )
		pluginManager->DoIdleProcessing();
}

//=================================================================================
void Frame::OnUndo( wxCommandEvent& event )
{
	Panel* panel = ( Panel* )noteBook->GetCurrentPage();
	if( panel )
	{
		UndoRedo* undoRedo = panel->GetUndoRedoSystem();
		undoRedo->Undo( panel );

		panel->RefreshUserInterface();
		UpdateUndoRedoToolBar();
	}
}

//=================================================================================
void Frame::OnRedo( wxCommandEvent& event )
{
	Panel* panel = wxDynamicCast( noteBook->GetCurrentPage(), Panel );
	if( panel )
	{
		UndoRedo* undoRedo = panel->GetUndoRedoSystem();
		undoRedo->Redo( panel );

		panel->RefreshUserInterface();
		UpdateUndoRedoToolBar();
	}
}

//=================================================================================
void Frame::OnSearch( wxCommandEvent& event )
{
	PerformSearch();
}

//=================================================================================
void Frame::OnSearchBoxEnter( wxCommandEvent& event )
{
	PerformSearch();
}

//=================================================================================
void Frame::PerformSearch( void )
{
	wxString searchCriteria = searchBox->GetValue();
	if( !searchCriteria.IsEmpty() )
	{
		// Cache the search criteria in the drop-down in case the user wants to use it again.
		if( wxNOT_FOUND == searchBox->FindString( searchCriteria, true ) )
			searchBox->Append( searchCriteria );

		// Go perform the search.
		Panel* panel = wxDynamicCast( noteBook->GetCurrentPage(), Panel );
		if( !panel )
			wxMessageBox( wxT( "Searching is not yet implemented for the currently focused panel." ), wxT( "Error" ), wxOK | wxCENTRE );
		else if( !panel->FindNext( searchCriteria ) )
			wxMessageBox( wxT( "Failed to find \"" ) + searchCriteria + wxT( "\", or anything that matches it, in the currently focused panel." ), wxT( "Error" ), wxOK | wxCENTRE );
	}
}

//=================================================================================
void Frame::UpdateUndoRedoToolBar( void )
{
	bool canUndo = false;
	bool canRedo = false;

	// This freeze/thaw gets rid of flicker in the tool-bar, but creates more flicker in the OpenGL panels, so remove it.
	//Freeze();

	Panel* panel = wxDynamicCast( noteBook->GetCurrentPage(), Panel );
	if( panel )
	{
		UndoRedo* undoRedo = panel->GetUndoRedoSystem();
		canUndo = undoRedo->CanUndo();
		canRedo = undoRedo->CanRedo();	
	}

	undoRedoToolBar->EnableTool( ID_Undo, canUndo );
	undoRedoToolBar->EnableTool( ID_Redo, canRedo );

	undoRedoToolBar->Realize();
	auiManager.Update();

	//Thaw();
}

//=================================================================================
void Frame::UpdateFileToolBar( void )
{
	// A better program would only enable the save button if
	// the currently open file has changes that are not on disk,
	// but I'm not going to worry too much about that right now.
	bool canSave = noteBook->GetPageCount() > 0 ? true : false;
	fileToolBar->EnableTool( ID_SaveTree, canSave );
	fileToolBar->Realize();
}

//=================================================================================
void Frame::UpdateSearchBar( void )
{
	bool canSearch = noteBook->GetPageCount() > 0 ? true : false;

	/*
	bool canSearch = false;
	Panel* panel = wxDynamicCast( noteBook->GetCurrentPage(), Panel );
	if( panel )
	{
		Tree* tree = panel->GetTree();
		if( tree->GetRoot() )
			canSearch = true;
	}
	*/

	searchToolBar->EnableTool( ID_Search, canSearch );
	searchToolBar->Realize();
	searchBox->Enable( canSearch );
}

//=================================================================================
void Frame::UpdateMenuBar( void )
{
	bool canSave = noteBook->GetPageCount() > 0 ? true : false;
	menuBar->FindItem( ID_SaveTree )->Enable( canSave );
}

//=================================================================================
Panel* Frame::CreatePanel( Tree* tree, UndoRedo* undoRedo /*= 0*/ )
{
	wxSize size = GetClientSize();
	wxPoint pos = wxDefaultPosition;
	Panel* panel = 0;
	switch( wxGetApp().GetInterfaceStyle() )
	{
		case Application::IFACE_STYLE_CONTROL_TREE:
		{
			panel = new ControlTreePanel( noteBook, tree );
			break;
		}
		case Application::IFACE_STYLE_DOCKABLE_PANELS:
		{
			panel = new DockingPanel( noteBook, tree, pos, size );
			break;
		}
		case Application::IFACE_STYLE_PROPERTY_GRID:
		{
			panel = new PropertyGridPanel( noteBook, tree );
			break;
		}
	}

	if( panel )
	{
		wxBusyCursor busyCursor;

		// File watcher events should be sent to the frame.
#if 0
		tree->SetOwner( this );
#endif

		panel->Freeze();
		panel->BuildUserInterface();

		// Give plugins a chance to augment the page UI while inside the freeze/thaw block.
		PluginManager* pluginManager = wxGetApp().GetPluginManager();
		if( pluginManager )
		{
			PluginPageEvent event;
			event.SetEventType( CORNUCOPIA_PLUGIN_PAGE_AUGMENT_UI );
			event.SetEventObject( this );
			event.SetPage( panel );
			pluginManager->CallAllPlugins( event );
		}

		panel->Thaw();

		if( !undoRedo )
			undoRedo = new UndoRedo( tree );

		panel->SetUndoRedoSystem( undoRedo );

		// Let all plugins know about the newly created page.
		// It hasn't actually been added to the note-book control yet,
		// but this is a good place to notify the plugins.
		if( pluginManager )
		{
			PluginPageEvent event;
			event.SetEventType( CORNUCOPIA_PLUGIN_PAGE_CREATED );
			event.SetEventObject( this );
			event.SetPage( panel );
			pluginManager->CallAllPlugins( event );
		}

		// Also send a full update tweak event on newly created/loaded trees.
		// Should we also send a full update on undo/redo?  Hmmm.  We may have no choice.
		if( pluginManager )
		{
			PluginTweakEvent event;
			event.SetEventType( CORNUCOPIA_PLUGIN_TWEAK_FULL_UPDATE );
			event.SetEventObject( this );
			event.SetNode( tree->GetRoot() );
			pluginManager->CallAllPlugins( event );
		}
	}

	return panel;
}

//=================================================================================
void Frame::OnFileWatchEvent( wxFileSystemWatcherEvent& event )
{
	wxFileName fileName = event.GetPath();
	Panel* panel = noteBook->FindPanel( fileName.GetPath() );
	if( !panel )
		return;

	switch( event.GetChangeType() )
	{
		// We should stop watching whenever the file is modified on disk internally (e.g. by a save operation),
		// so whenever we receive this event, it should be because of an external modification of the file on disk.
		case wxFSW_EVENT_MODIFY:
		{
			( void )ReopenTree( panel, wxString( "Cornucopia Editor has detected an external modification of the file \"" ) + fileName.GetPath() + wxString( "\".  Do you want to reload this file?" ) );
			break;
		}
		case wxFSW_EVENT_DELETE:
		{
			int response = wxMessageBox( wxString( "Cornucopia Editor has detected that the file \"" ) + fileName.GetPath() + wxString( "\" has been deleted externally.  Do you want to keep this file in the editor?" ), wxT( "External Delete Detected" ), wxYES_NO | wxCENTRE );
			if( response == wxNO )
			{
				int pageIndex = noteBook->GetPageIndex( panel );
				if( pageIndex != wxNOT_FOUND )
					noteBook->DeletePage( pageIndex );
			}
			else
			{
				// Mark the file as modified so that we don't forget to save the tree before exit.
				Tree* tree = panel->GetTree();
				tree->SetModified( true );
			}
			break;
		}
	}
}

//=================================================================================
void Frame::OnNewTree( wxCommandEvent& event )
{
	Freeze();

	// Create a blank tree that can be edited by this application.
	Tree* tree = new Tree();
	Panel* panel = CreatePanel( tree );
	noteBook->AddPage( panel, tree->GetTitle(), true );
	panel->PostOpen();

	Thaw();
}

//=================================================================================
void Frame::OnNewClone( wxCommandEvent& event )
{
	Freeze();

	do
	{
		// Query the user for the tree to clone.
		wxFileDialog fileDialog( this, wxT( "Clone Cornucopia Tree" ), wxEmptyString, wxEmptyString, wxT( "Cornucopia Files (*.lua)|*.lua" ), wxFD_OPEN | wxFD_FILE_MUST_EXIST );
		if( fileDialog.ShowModal() == wxID_CANCEL )
			break;

		// Try to open the chosen tree.
		wxString filePath = fileDialog.GetPath();
		wxScopedPtr< Tree > tree;
		tree.reset( new Tree() );
		if( !tree->Open( filePath ) )
		{
			wxGetApp().CornucopiaErrorMessageBox( wxString::Format( wxT( "Failed to open file \"%s\"." ), filePath ) );
			break;
		}

		// Try to clone the chosen tree.
		wxScopedPtr< Cornucopia::Node > node;
		Cornucopia::Node::CopyParameters copyParameters;
		node.reset( tree.get()->GetRoot()->Clone( wxGetApp().GetContext(), copyParameters ) );
		if( !node.get() )
		{
			wxGetApp().CornucopiaErrorMessageBox( wxT( "Failed to cloned tree!" ) );
			break;
		}

		// Bring up the newly cloned tree in the UI.
		Tree* clonedTree = new Tree();
		clonedTree->SetRoot( node.release() );
		Panel* panel = CreatePanel( clonedTree );
		noteBook->AddPage( panel, clonedTree->GetTitle(), true );
		panel->PostOpen();
	}
	while( false );

	Thaw();
}

//=================================================================================
void Frame::OnOpenTree( wxCommandEvent& WXUNUSED( event ) )
{
	Freeze();

	// By passing an empty string, we signal the routine to query the user for a file path.
	wxString filePath( wxT( "" ) );
	( void )OpenTree( filePath );

	Thaw();
}

//=================================================================================
bool Frame::OpenTree( wxString& filePath )
{
	// Let the user select a Lua file from disk if signaled to do so.
	if( filePath.IsEmpty() )
	{
		wxFileDialog fileDialog( this, wxT( "Open Cornucopia Tree" ), wxEmptyString, wxEmptyString, wxT( "Cornucopia Files (*.lua)|*.lua" ), wxFD_OPEN | wxFD_FILE_MUST_EXIST );
		if( fileDialog.ShowModal() == wxID_CANCEL )
			return true;

		filePath = fileDialog.GetPath();
	}

	// If the file is already opened by the editor, just flip to that page.
	// We would not want to have the file open twice, though the user could
	// do this by using multiple instances of the application.
	Panel* panel = noteBook->FindPanel( filePath );
	if( panel )
	{
		noteBook->SetSelection( noteBook->GetPageIndex( panel ) );
		return true;
	}

	// Try to load the given file or the one selected by the user.
	wxScopedPtr< Tree > tree;
	tree.reset( new Tree() );
	if( !tree->Open( filePath ) )
	{
		wxGetApp().CornucopiaErrorMessageBox( wxString::Format( wxT( "Failed to open file \"%s\"." ), filePath ) );
		return false;
	}

	// Create and add a new panel to the note-book that will
	// act as an interface to the newly loaded Cornucopia tree.
	panel = CreatePanel( tree.get() );
	noteBook->AddPage( panel, tree.get()->GetTitle(), true );
	tree.release();
	panel->PostOpen();
	return true;
}

//=================================================================================
// Note that our application has no need of a "save as" option, because that
// functionality is provided by an ability to clone an existing tree, which
// initially has no file name associated with it, and then to save that tree.
// We'll ask for a save location if the associated tree doesn't already have one.
void Frame::OnSaveTree( wxCommandEvent& WXUNUSED( event ) )
{
	// Grab the panel in focus.
	Panel* panel = wxDynamicCast( noteBook->GetCurrentPage(), Panel );
	if( !panel )
		return;

	// Try to save its tree.
	( void )SaveTree( panel );
}

//=================================================================================
bool Frame::SaveTree( Panel* panel )
{
	// Grab the panel's tree.  Make sure it has a save location.
	Tree* tree = panel->GetTree();
	if( tree->GetFilePath().IsEmpty() )
	{
		// Ask the user for a save location, because we don't have one.
		wxString filePath;
		while( true )
		{
			wxFileDialog fileDialog( this, wxT( "Save Cornucopia Tree" ), wxEmptyString, wxEmptyString, wxT( "Cornucopia Files (*.lua)|*.lua" ), wxFD_SAVE | wxFD_OVERWRITE_PROMPT );
			if( fileDialog.ShowModal() == wxID_CANCEL )
				return true;

			// Prevent saving to a location that is already opened by another panel in the application.
			filePath = fileDialog.GetPath();
			Panel* foundPanel = noteBook->FindPanel( filePath );
			if( foundPanel && foundPanel != panel )
				wxMessageBox( wxString::Format( wxT( "The file \"%s\" is already open.  To prevent duplicates interfering with one another, please choose a different save location." ), filePath ), wxT( "Error" ), wxOK | wxCENTRE );
			else
				break;
		}

		// Go ahead and assign the new path.
		tree->SetFilePath( filePath );
		noteBook->UpdateTabTitle( noteBook->GetSelection() );
	}

	// Write out the tree if it's modified.  (If it's not, then we
	// can assume that the tree is unmodified from what it is on disk.)
	if( tree->GetModified() )
	{
		// If we don't have permissions to overwrite the file, try deferring to a plugin.
		wxFileName fileName( tree->GetFilePath() );
		bool fileExists = fileName.Exists();
		if( fileExists && !fileName.IsFileWritable() )
		{
			PluginManager* pluginManager = wxGetApp().GetPluginManager();
			if( pluginManager )
			{
				PluginFileEvent event;
				event.SetEventType( CORNUCOPIA_PLUGIN_FILE_RESOLVE_NONWRITABLE );
				event.SetEventObject( this );
				event.SetFile( fileName.GetFullPath() );
				pluginManager->CallAllPlugins( event );
			}

			if( !fileName.IsFileWritable() )
			{
				wxMessageBox( wxString::Format( wxT( "The file \"%s\" is read-only and therefore cannot be rewritten." ), fileName.GetFullPath() ), wxT( "Error" ), wxOK | wxCENTRE );
				return false;
			}
		}

		panel->PreSave();

		if( !tree->Save() )
		{
			wxGetApp().CornucopiaErrorMessageBox( wxString::Format( wxT( "Failed to save file \"%s\"." ), tree->GetFilePath() ) );
			return false;
		}

		noteBook->UpdateTabTitle( panel );

		// If we just created a new file on disk, tell the plugins about it.
		if( !fileExists )
		{
			PluginManager* pluginManager = wxGetApp().GetPluginManager();
			if( pluginManager )
			{
				PluginFileEvent event;
				event.SetEventType( CORNUCOPIA_PLUGIN_FILE_CREATED );
				event.SetEventObject( this );
				event.SetFile( fileName.GetFullPath() );
				pluginManager->CallAllPlugins( event );
			}
		}
	}

	return true;
}

//=================================================================================
bool Frame::ReopenTree( Panel* panel, const wxString& prompt )
{
	int response = wxMessageBox( prompt, wxT( "Reload Request" ), wxYES_NO | wxCENTRE );
	if( response != wxYES )
		return false;

	bool success = false;

	Freeze();

	do
	{
		Tree* tree = panel->GetTree();
		wxString filePath = tree->GetFilePath();

		int pageIndex = noteBook->GetPageIndex( panel );
		if( pageIndex == wxNOT_FOUND )
			break;

		if( !noteBook->DeletePage( pageIndex ) )
			break;
	
		if( !OpenTree( filePath ) )
			break;

		success = true;
	}
	while( false );

	Thaw();

	return success;
}

//=================================================================================
void Frame::OnUpgradeTrees( wxCommandEvent& event )
{
	wxDirDialog dirDialog( this, wxT( "Select the folder containing the files to be upgraded." ), wxEmptyString, wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST );
	if( wxID_OK != dirDialog.ShowModal() )
		return;

	Upgrader upgrader;
	wxDir dir( dirDialog.GetPath() );
	dir.Traverse( upgrader, wxT( "*.lua" ), wxDIR_DIRS | wxDIR_FILES );
	upgrader.PerformUpgrade();

	wxMessageDialog messageDialog( this, wxT( "Upgrade complete!" ), wxT( "Upgrade Report" ) );
	messageDialog.SetExtendedMessage( upgrader.GetReport() );
	messageDialog.ShowModal();
}

//=================================================================================
void Frame::OnWipeTreeCache( wxCommandEvent& event )
{
	wxGetApp().GetContext().WipeTreeCache();
}

//=================================================================================
void Frame::OnTouchTree( wxCommandEvent& event )
{
	wxWindow* currentPage = noteBook->GetCurrentPage();
	Panel* panel = wxDynamicCast( currentPage, Panel );
	if( panel )
	{
		Tree* tree = panel->GetTree();
		tree->SetModified( true );
		noteBook->UpdateTabTitle( panel );
	}
}

//=================================================================================
void Frame::OnExit( wxCommandEvent& event )
{
	Close( true );
}

//=================================================================================
void Frame::OnClose( wxCloseEvent& event )
{
	// Before exiting the application, be sure to give the user a chance
	// to save any unsaved work.
	int pageCount = noteBook->GetPageCount();
	for( int pageIndex = 0; pageIndex < pageCount; pageIndex++ )
	{
		Panel* panel = wxDynamicCast( noteBook->GetPage( pageIndex ), Panel );
		if( panel && LastChanceToSavePanel( panel ) )
			return;		// Abort application closure.
	}

	// Okay, the program is going down.  Save our settings.
	// Notice that we do this even if app settings were suppressed.
	// They may have been suppressed because they were corrupt, so here we may uncorrupt them.
	Application::AppSettings appSettings;
	wxGetApp().PopulateAppSettings( appSettings );
	wxGetApp().SaveAppSettings( appSettings );

	// Unload all plugins before the UI goes down.
	PluginManager* pluginManager = wxGetApp().GetPluginManager();
	if( pluginManager )
		pluginManager->UnloadAllPlugins();

	// Proceed with application closure.
	// Let wxWidget's handle the event to actually close the window.
	event.Skip();
}

//=================================================================================
wxString Frame::GetPerspective( void )
{
	return auiManager.SavePerspective();
}

//=================================================================================
void Frame::SetPerspective( const wxString& perspective )
{
	if( !perspective.IsEmpty() )
		auiManager.LoadPerspective( perspective, true );

	// Loading a perspective can hide newly created tool-bars,
	// so lets go make sure that they're all visible.
	bool paneShown = false;
	int paneCount = auiManager.GetAllPanes().size();
	for( int index = 0; index < paneCount; index++ )
	{
		wxAuiPaneInfo& paneInfo = auiManager.GetAllPanes()[ index ];
		if( paneInfo.IsToolbar() && !paneInfo.IsShown() )
		{
			paneShown = true;
			paneInfo.Show();
		}
	}

	if( paneShown )
		auiManager.Update();
}

//=================================================================================
// A return value of true here signals an abort of whatever operation is
// about to close the panel.  This could be an attempt to exit the application,
// or to simply close down the panel.
bool Frame::LastChanceToSavePanel( Panel* panel )
{
	Tree* tree = panel->GetTree();
	if( tree->GetModified() )
	{
		wxString filePath = tree->GetFilePath();
		if( filePath.IsEmpty() )
			filePath = noteBook->GetPageText( noteBook->GetPageIndex( panel ) );
		int response = wxMessageBox( wxString::Format( wxT( "The opened file \"%s\" has unsaved changed.  Save now?" ), filePath ), wxT( "Warning" ), wxYES_NO | wxCANCEL | wxCENTRE );
		if( response == wxCANCEL )
			return true;
		else if( response == wxYES )
		{
			if( !SaveTree( panel ) )
			{
				//...
			}
		}
	}

	return false;
}

//=================================================================================
void Frame::OnHelp( wxCommandEvent& event )
{
	ActivateDocumentationPanel();
}

//=================================================================================
void Frame::ActivateDocumentationPanel( const wxString& initialPage /*= wxEmptyString*/ )
{
	if( helpPanel )
		helpPanel->GoToPage( initialPage );
	else
	{
		helpPanel = new HelpPanel( noteBook, initialPage );
		noteBook->AddPage( helpPanel, wxEmptyString );
		helpPanel->UpdateUI();
	}
	
	int pageIndex = noteBook->GetPageIndex( helpPanel );
	noteBook->SetSelection( pageIndex );
}

//=================================================================================
void Frame::OnPluginManager( wxCommandEvent& event )
{
	if( !wxGetApp().GetPluginManager() )
		wxMessageBox( wxT( "The plugin manager has been perminantly disabled for this invocation of the application." ), wxT( "Error" ), wxOK | wxCENTRE );
	else
	{
		if( !pluginPanel )
		{
			pluginPanel = new PluginPanel( noteBook );
			noteBook->AddPage( pluginPanel, wxT( "Plugin Manager" ) );
		}

		int pageIndex = noteBook->GetPageIndex( pluginPanel );
		noteBook->SetSelection( pageIndex );
	}
}

//=================================================================================
PluginPanel* Frame::GetPluginPanel( void )
{
	return pluginPanel;
}

//=================================================================================
void Frame::NotifyOfImminentPageClosure( wxWindow* page )
{
	// We don't own the memory, so let go of our pointer to it.
	if( page == helpPanel )
		helpPanel = 0;
	else if( page == pluginPanel )
		pluginPanel = 0;
}

//=================================================================================
void Frame::OnCheckUniqueness( wxCommandEvent& event )
{
	wxWindow* page = noteBook->GetCurrentPage();
	Panel* panel = wxDynamicCast( page, Panel );
	if( !panel )
	{
		wxMessageBox( wxT( "You must select a page that edits a Cornucopia Tree before selecting this option." ), wxT( "Error" ), wxOK | wxCENTRE );
		return;
	}

	Tree* tree = panel->GetTree();
	std::list< std::string > nonUniqueNameList;
	tree->PopulateNonUniqueNameList( nonUniqueNameList );
	if( nonUniqueNameList.size() == 0 )
		wxMessageBox( wxString::Format( "All names within the Cornucopia Tree for file \"%s\" are unique!", ( const char* )tree->GetTitle().c_str() ), wxT( "Result" ), wxOK | wxCENTRE );
	else
	{
		wxString shortMessage = wxString::Format( "Not all names within the Cornucopia Tree for file \"%s\" are unique!  The duplicated names are as follows.", ( const char* )tree->GetTitle().c_str() );
		
		wxString extendedMessage = wxT( "" );
		for( std::list< std::string >::iterator iter = nonUniqueNameList.begin(); iter != nonUniqueNameList.end(); iter++ )
		{
			std::string nodeName = *iter;
			extendedMessage += wxString( ( const char* )nodeName.c_str() ) + wxT( "\n" );
		}

		wxMessageDialog messageDialog( this, shortMessage, wxT( "Results" ), wxOK | wxCENTRE );
		messageDialog.SetExtendedMessage( extendedMessage );
		messageDialog.ShowModal();
	}
}

//=================================================================================
void Frame::OnAbout( wxCommandEvent& event )
{
	wxAboutDialogInfo aboutDlgInfo;

	aboutDlgInfo.SetName( wxT( "Cornucopia Editor" ) );
	aboutDlgInfo.SetVersion( wxT( "v1.0" ) );
	aboutDlgInfo.SetDescription( wxT( "Please send bugs to spencerparkin@outlook.com.\nComments and questions are welcome.\nThank you for your interest in this program." ) );
	aboutDlgInfo.SetCopyright( wxT( "Copyright (C) 2013" ) );
	aboutDlgInfo.SetWebSite( wxT( "https://github.com/spencerparkin/CornucopiaEditor" ) );
	aboutDlgInfo.AddDeveloper( wxT( "Spencer T. Parkin" ) );
	aboutDlgInfo.SetLicense( wxT( "MIT License" ) );
	aboutDlgInfo.SetIcon( icon );

	wxAboutBox( aboutDlgInfo );
}

// Frame.cpp