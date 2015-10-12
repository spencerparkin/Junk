// NoteBook.cpp

#include "CornucopiaEditor.h"

using namespace CornucopiaEditor;

//=================================================================================
BEGIN_EVENT_TABLE( NoteBook, wxAuiNotebook )

	EVT_AUINOTEBOOK_PAGE_CHANGED( wxID_ANY, OnPageChanged )
	EVT_AUINOTEBOOK_PAGE_CLOSE( wxID_ANY, OnPageClose )
	EVT_AUINOTEBOOK_PAGE_CLOSED( wxID_ANY, OnPageClosed )

	EVT_AUINOTEBOOK_TAB_RIGHT_DOWN( wxID_ANY, OnTabRightDown )

	EVT_MENU( ID_ContextMenu_DockableStyleInterface, OnContextMenu )
	EVT_MENU( ID_ContextMenu_ControlTreeStyleInterface, OnContextMenu )
	EVT_MENU( ID_ContextMenu_PropertyGridStyleInterface, OnContextMenu )

END_EVENT_TABLE()

//=================================================================================
NoteBook::NoteBook( wxWindow* parent ) : wxAuiNotebook( parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
			wxAUI_NB_TOP | wxAUI_NB_TAB_SPLIT | wxAUI_NB_TAB_MOVE | wxAUI_NB_SCROLL_BUTTONS | wxAUI_NB_CLOSE_ON_ALL_TABS | wxAUI_NB_WINDOWLIST_BUTTON )
{
	contextPageIndex = wxNOT_FOUND;
}

//=================================================================================
/*virtual*/ NoteBook::~NoteBook( void )
{
}

//=================================================================================
Panel* NoteBook::FindPanel( const wxString& filePath )
{
	wxFileName givenFileName( filePath );

	int pageCount = GetPageCount();
	for( int pageIndex = 0; pageIndex < pageCount; pageIndex++ )
	{
		Panel* panel = wxDynamicCast( GetPage( pageIndex ), Panel );
		if( panel )
		{
			Tree* tree = panel->GetTree();
			wxFileName fileName( tree->GetFilePath() );

			// This will normalize each file before comparing.
			if( fileName == givenFileName )
				return panel;
		}
	}

	return 0;
}

//=================================================================================
Tree* NoteBook::FindTree( const wxString& filePath )
{
	Panel* panel = FindPanel( filePath );
	if( panel )
		return panel->GetTree();
	return 0;
}

//=================================================================================
void NoteBook::OnPageClose( wxAuiNotebookEvent& event )
{
	Frame* frame = wxGetApp().GetFrame();
	frame->Freeze();		// Subsequent thaw happens in OnPageClosed!

	int pageIndex = event.GetSelection();
	if( pageIndex != wxNOT_FOUND )
	{
		wxWindow* page = GetPage( pageIndex );
		Panel* panel = wxDynamicCast( page, Panel );
		if( panel && wxGetApp().GetFrame()->LastChanceToSavePanel( panel ) )
		{
			event.Veto();
			return;
		}

		if( page )
			frame->NotifyOfImminentPageClosure( page );
	}

	PluginManager* pluginManager = wxGetApp().GetPluginManager();
	if( pluginManager )
	{
		PluginPageEvent event;
		event.SetEventType( CORNUCOPIA_PLUGIN_PAGE_DESTROYING );
		event.SetEventObject( this );
		event.SetPage( pageIndex == wxNOT_FOUND ? 0 : GetPage( pageIndex ) );
		pluginManager->CallAllPlugins( event );
	}
}

//=================================================================================
void NoteBook::OnPageClosed( wxAuiNotebookEvent& event )
{
	Panel* panel = wxDynamicCast( GetCurrentPage(), Panel );
	UpdateTabTitle( panel );

	Frame* frame = wxGetApp().GetFrame();
	frame->UpdateUndoRedoToolBar();
	frame->UpdateFileToolBar();
	frame->UpdateMenuBar();
	frame->UpdateSearchBar();

	if( GetPageCount() == 0 )
	{
		PluginManager* pluginManager = wxGetApp().GetPluginManager();
		if( pluginManager )
		{
			PluginPageEvent event;
			event.SetEventType( CORNUCOPIA_PLUGIN_PAGE_CHANGED );
			event.SetEventObject( this );
			event.SetPage(0);
			pluginManager->CallAllPlugins( event );
		}
	}

	// This fixes the same glitch as commented about in OnPageChanged.
	frame->GetAuiManager()->Update();
	frame->Thaw();		// Thaw from freeze that happened in OnPageClose!
}

//=================================================================================
void NoteBook::OnPageChanged( wxAuiNotebookEvent& event )
{
	int pageIndex = event.GetSelection();
	if( pageIndex != wxNOT_FOUND )
		UpdateTabTitle( pageIndex );

	Frame* frame = wxGetApp().GetFrame();
	frame->UpdateUndoRedoToolBar();
	frame->UpdateFileToolBar();
	frame->UpdateMenuBar();
	frame->UpdateSearchBar();

	PluginManager* pluginManager = wxGetApp().GetPluginManager();
	if( pluginManager )
	{
		PluginPageEvent event;
		event.SetEventType( CORNUCOPIA_PLUGIN_PAGE_CHANGED );
		event.SetEventObject( this );
		event.SetPage( pageIndex == wxNOT_FOUND ? 0 : GetPage( pageIndex ) );
		pluginManager->CallAllPlugins( event );
	}

	// This call fixes a graphics glitch caused, not by the rendering code
	// having a bug, but, I believe, by the layout code not quite working
	// whenever it needs to be.  This will force a layout recalculation
	// and then a subsequent repaint to fix the glitch.
	frame->GetAuiManager()->Update();
}

//=================================================================================
void NoteBook::UpdateTabTitle( int pageIndex )
{
	Panel* panel = wxDynamicCast( GetPage( pageIndex ), Panel );
	if( panel )
		UpdateTabTitle( panel );
}

//=================================================================================
void NoteBook::UpdateTabTitle( Panel* panel )
{
	wxString title;
	if( panel )
	{
		Tree* tree = panel->GetTree();
		title = tree->GetTitle();
	
		int pageIndex = GetPageIndex( panel );
		SetPageText( pageIndex, title );
	}

	// We also update the frame title, since it should reflect the current tab title.
	if( GetCurrentPage() == panel )
	{
		wxWindow* window = GetParent();
		if( window )
		{
			window = window->GetParent();
			Frame* frame = wxDynamicCast( window, Frame );
			if( frame )
			{
				Frame* frame = ( Frame* )window;
				if( panel )
					frame->SetTitle( title + wxT( " -- Cornucopia Editor" ) );
				else
					frame->SetTitle( wxT( "Cornucopia Editor" ) );
			}
		}
	}
}

//=================================================================================
void NoteBook::OnTabRightDown( wxAuiNotebookEvent& event )
{
	contextPageIndex = event.GetSelection();
	if( contextPageIndex == wxNOT_FOUND )
		return;
	
	Panel* panel = wxDynamicCast( GetPage( contextPageIndex ), Panel );
	if( !panel )
		return;
	
	wxMenu contextMenu;
	wxMenuItem* dockableStyleInterfaceItem = new wxMenuItem( &contextMenu, ID_ContextMenu_DockableStyleInterface, wxT( "Dockable Style Interface" ), wxT( "Use the Cornucopia tree interface consisting of a data-view tree control with floating/dockable panels giving mini-interfaces to each node in the tree." ), wxITEM_CHECK );
	wxMenuItem* controlTreeStyleInterfaceItem = new wxMenuItem( &contextMenu, ID_ContextMenu_ControlTreeStyleInterface, wxT( "Control Tree Style Interface" ), wxT( "Use the Cornucopia tree interface consisting of nested control panels acting as mini-interfaces to each node in the tree." ), wxITEM_CHECK );
	wxMenuItem* propertyGridStyleInterfaceItem = new wxMenuItem( &contextMenu, ID_ContextMenu_PropertyGridStyleInterface, wxT( "Property Grid Style Interface" ), wxT( "Use the Cornucopia tree interface consisting of a property grid with nested property." ), wxITEM_CHECK );
	contextMenu.Append( dockableStyleInterfaceItem );
	contextMenu.Append( controlTreeStyleInterfaceItem );
	contextMenu.Append( propertyGridStyleInterfaceItem );
	dockableStyleInterfaceItem->Check( wxDynamicCast( panel, DockingPanel ) != 0 );
	controlTreeStyleInterfaceItem->Check( wxDynamicCast( panel, ControlTreePanel ) != 0 );
	propertyGridStyleInterfaceItem->Check( wxDynamicCast( panel, PropertyGridPanel ) != 0 );

	wxMouseState mouseState = wxGetMouseState();
	wxPoint mousePos = mouseState.GetPosition();
	mousePos = ScreenToClient( mousePos );
	PopupMenu( &contextMenu, mousePos );
}

//=================================================================================
void NoteBook::OnContextMenu( wxCommandEvent& event )
{
	if( contextPageIndex == wxNOT_FOUND )
		return;

	Panel* panel = wxDynamicCast( GetPage( contextPageIndex ), Panel );
	if( !panel )
		return;

	// There's nothing for us to do if the interface chosen is already the interface at hand.
	if( ( event.GetId() == ID_ContextMenu_DockableStyleInterface && wxDynamicCast( panel, DockingPanel ) ) ||
		( event.GetId() == ID_ContextMenu_ControlTreeStyleInterface && wxDynamicCast( panel, ControlTreePanel ) ) ||
		( event.GetId() == ID_ContextMenu_PropertyGridStyleInterface && wxDynamicCast( panel, PropertyGridPanel ) ) )
	{
		return;
	}

	switch( event.GetId() )
	{
		case ID_ContextMenu_DockableStyleInterface:
		{
			wxGetApp().SetInterfaceStyle( Application::IFACE_STYLE_DOCKABLE_PANELS );
			break;
		}
		case ID_ContextMenu_ControlTreeStyleInterface:
		{
			wxGetApp().SetInterfaceStyle( Application::IFACE_STYLE_CONTROL_TREE );
			break;
		}
		case ID_ContextMenu_PropertyGridStyleInterface:
		{
			wxGetApp().SetInterfaceStyle( Application::IFACE_STYLE_PROPERTY_GRID );
			break;
		}
	}

	UndoRedo* undoRedo = panel->GetUndoRedoSystem();
	panel->SetUndoRedoSystem( 0, false );

	Tree* tree = panel->GetTree( true );
	RemovePage( contextPageIndex );
	panel->Destroy();

	panel = wxGetApp().GetFrame()->CreatePanel( tree, undoRedo );
	AddPage( panel, tree->GetTitle(), true );
}

// NoteBook.cpp