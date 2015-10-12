// DockingPanel.cpp

#include "CornucopiaEditor.h"

using namespace CornucopiaEditor;

wxIMPLEMENT_DYNAMIC_CLASS( DockingPanel, Panel )

//=================================================================================
// This default constructor should never be used.
DockingPanel::DockingPanel( void )
{
	dataViewCtrl = 0;
}

//=================================================================================
DockingPanel::DockingPanel( wxWindow* parent, Tree* tree, const wxPoint& pos, const wxSize& size ) : Panel( parent, tree, pos, size )
{
	auiManager.SetManagedWindow( this );
	auiManager.SetArtProvider( new DockArt() );

	// Hmmm.  The live-resize feature only seems to work when the window is getting bigger, not smaller.
	auiManager.SetFlags( wxAUI_MGR_DEFAULT | wxAUI_MGR_LIVE_RESIZE );
}

//=================================================================================
/*virtual*/ DockingPanel::~DockingPanel( void )
{
	auiManager.UnInit();
}

//=================================================================================
/*virtual*/ void DockingPanel::BuildUserInterface( void )
{
	dataViewCtrl = new DataViewCtrl( this );
	dataViewCtrl->RebuildColumns();
	dataViewCtrl->SetIndent( 16 );

	RefreshDataViewControl();
	RefreshTreeControlExpansionState();
	RefreshColumnWidths();

	auiManager.AddPane( dataViewCtrl, wxAuiPaneInfo().Name( wxT( "TreeDataViewCtrl" ) ).Caption( wxT( "Tree Data View Control" ) ).CenterPane().PaneBorder( false ) );

	RestoreNodeControlPanels();

	// Notice that for our restored panel sizes to come up correctly,
	// our client size needs to be correct; otherwise, the docking
	// constraint will clamp the sizes incorrectly.
	auiManager.SetDockSizeConstraint( 2.0 / 3.0, 2.0 / 3.0 );
	auiManager.Update();
}

//=================================================================================
/*virtual*/ void DockingPanel::RefreshUserInterface( void )
{
	//Freeze();

	RefreshDataViewControl();
	RefreshTreeControlExpansionState();
	RefreshColumnWidths();
	RefreshNodeControlPanels();

	NoteBook* noteBook = wxGetApp().GetFrame()->GetNoteBook();
	int pageIndex = noteBook->GetPageIndex( this );
	noteBook->UpdateTabTitle( pageIndex );

	//Thaw();
}

//=================================================================================
void DockingPanel::RefreshDataViewControl( void )
{
	// Rebuild the tree control.  Admittedly, depending on what
	// changed that requires a refresh, this may be overkill, and
	// is potentially a performance problem if dealing with a big tree.
	// If just one or a few items change, those items should be updated
	// instead of rebuilding the entire tree.
	wxObjectDataPtr< DataViewModel > dataViewModel;
	dataViewModel = new DataViewModel( tree, dataViewCtrl );
	dataViewCtrl->AssociateModel( dataViewModel.get() );	// Being ref-counted, the replaced model, if any, should be destroyed by this.
}

//=================================================================================
void DockingPanel::RefreshTreeControlExpansionState( void )
{
	Cornucopia::Node* node = tree->GetRoot();
	if( !node )
		return;

	Cornucopia::Node::List nodeList;
	node->MakeNodeList( nodeList );

	for( Cornucopia::Node::List::iterator iter = nodeList.begin(); iter != nodeList.end(); iter++ )
	{
		node = *iter;

		EditorMetaData* editorMetaData = ( EditorMetaData* )node->GetMetaData();
		bool expanded = editorMetaData->GetExpanded();

		wxDataViewItem item( node );
		if( expanded )
			dataViewCtrl->Expand( item );
		else
			dataViewCtrl->Collapse( item );
	}
}

//=================================================================================
void DockingPanel::RefreshColumnWidths( void )
{
	// This subroutine exposed what might be a bug in wxWidgets.
	// The data view control will auto-size correctly according
	// to the first row and the column headers, but not take into
	// account all other rows.  It could be a cache invalidation problem.
	return;

	int columnCount = dataViewCtrl->GetColumnCount();
	for( int columnIndex = 0; columnIndex < columnCount; columnIndex++ )
	{
		wxDataViewColumn* column = dataViewCtrl->GetColumn( columnIndex );
		column->SetWidth( wxCOL_WIDTH_AUTOSIZE );
	}
}

//=================================================================================
bool DockingPanel::IsNodeControlPanelActiveForNode( Cornucopia::Node* node )
{
	int paneCount = auiManager.GetAllPanes().GetCount();
	for( int paneIndex = 0; paneIndex < paneCount; paneIndex++ )
	{
		wxAuiPaneInfo& paneInfo = auiManager.GetAllPanes()[ paneIndex ];
		NodeControlPanel* nodeControlPanel = wxDynamicCast( paneInfo.window, NodeControlPanel );
		if( nodeControlPanel && nodeControlPanel->GetNode() == node )
			return true;
	}
	return false;
}

//=================================================================================
void DockingPanel::RestoreNodeControlPanels( void )
{
	Tree* tree = GetTree();
	if( tree )
	{
		Cornucopia::Node* root = tree->GetRoot();
		if( root )
		{
			Cornucopia::Node::List nodeList;
			root->MakeNodeList( nodeList );
			for( Cornucopia::Node::List::iterator iter = nodeList.begin(); iter != nodeList.end(); iter++ )
			{
				Cornucopia::Node* node = *iter;
				EditorMetaData* editorMetaData = ( EditorMetaData* )node->GetMetaData();
				
				// Ignore what handle we may have.  It may not be valid anyway,
				// and even if it is, the node control panel's node pointer
				// may not be patched correctly, and may even be stale.
				editorMetaData->SetNodeControlPanel(0);

				bool shouldActivateNodeControlPanel = editorMetaData->GetShouldActivateNodeControlPanel();
				if( shouldActivateNodeControlPanel )
					ActivateNodeControlPanelForNode( node, false, true );
			}
		}
	}
}

//=================================================================================
/*virtual*/ void DockingPanel::PreSave( void )
{
	Cornucopia::Node* root = 0;
	Tree* tree = GetTree();
	if( tree )
		root = tree->GetRoot();
	if( root )
	{
		Cornucopia::Node::List nodeList;
		root->MakeNodeList( nodeList );
		for( Cornucopia::Node::List::iterator iter = nodeList.begin(); iter != nodeList.end(); iter++ )
		{
			Cornucopia::Node* node = *iter;
			SaveNodeControlPanelInfo( node );
		}
	}
}

//=================================================================================
void DockingPanel::SaveNodeControlPanelInfo( Cornucopia::Node* node )
{
	EditorMetaData* editorMetaData = ( EditorMetaData* )node->GetMetaData();
	NodeControlPanel* nodeControlPanel = editorMetaData->GetNodeControlPanel();
	editorMetaData->SetShouldActivateNodeControlPanel( nodeControlPanel ? true : false );
	if( nodeControlPanel )
	{
		wxAuiPaneInfo paneInfo = auiManager.GetPane( nodeControlPanel );
		paneInfo.BestSize( paneInfo.rect.width, paneInfo.rect.height );
		wxString paneInfoString = auiManager.SavePaneInfo( paneInfo );
		editorMetaData->SetPaneInfoString( paneInfoString );
	}
}

//=================================================================================
/*virtual*/ void DockingPanel::PreUndoRedoNotice( Cornucopia::Node* newTree, Cornucopia::Node* forgottenTree )
{
	if( forgottenTree )
	{
		Cornucopia::Node* root = GetTree()->GetRoot();
		wxASSERT( root != 0 );

		// We should never fail to create this bridge.
		bool bridgeCreated = CreateBridge( root, forgottenTree );
		wxASSERT( bridgeCreated );
		if( bridgeCreated )
		{
			Cornucopia::Node::List nodeList;
			root->MakeNodeList( nodeList );
			for( Cornucopia::Node::List::iterator iter = nodeList.begin(); iter != nodeList.end(); iter++ )
			{
				Cornucopia::Node* node = *iter;
				SaveNodeControlPanelInfo( node );

				EditorMetaData* editorMetaData = ( EditorMetaData* )node->GetMetaData();
				bool shouldActivateNodeControlPanel = editorMetaData->GetShouldActivateNodeControlPanel();
				wxString paneInfoString = editorMetaData->GetPaneInfoString();
				node = editorMetaData->GetBridgeNode();
				editorMetaData = ( EditorMetaData* )node->GetMetaData();
				editorMetaData->SetShouldActivateNodeControlPanel( shouldActivateNodeControlPanel );
				editorMetaData->SetPaneInfoString( paneInfoString );
			}
		}
	}
}

//=================================================================================
/*virtual*/ void DockingPanel::PostUndoRedoNotice( Cornucopia::Node* oldTree, Cornucopia::Node* forgottenTree )
{
	// If the tree structure didn't change, we can update our interface
	// without deactivating and activating panels.
	Cornucopia::Node* newTree = tree->GetRoot();
	if( CreateBridge( oldTree, newTree ) )
	{
		PatchNodeControlPanelsUsingBridge( oldTree );
		RefreshUserInterface();
	}
	else
	{
		// In this case we have to deal with deactivating old panels and activating new ones.
		if( oldTree )
			DeactivateNodeControlPanelsForSubTree( oldTree, false, true );
		RestoreNodeControlPanels();
		auiManager.Update();
	}

	// Be sure to inform the data view control so that it doesn't
	// try to give us stale pointers to dereference.
	wxDataViewModel* dataViewModel = dataViewCtrl->GetModel();
	dataViewModel->ItemDeleted( wxDataViewItem(0), wxDataViewItem( oldTree ) );
	if( newTree )
		dataViewModel->ItemAdded( wxDataViewItem(0), wxDataViewItem( newTree ) );
}

//=================================================================================
/*virtual*/ void DockingPanel::SynchronizeControlsWithNode( Cornucopia::Node* node )
{
	// Notice here that we're not asking the panel to
	// refresh the data view control.  That would be
	// over-kill.  We're just asking it to update one
	// of its items.
	wxDataViewModel* dataViewModel = dataViewCtrl->GetModel();
	wxDataViewItem changedItem( node );
	dataViewModel->ItemChanged( changedItem );
}

//=================================================================================
void DockingPanel::ActivateNodeControlPanelForNode( Cornucopia::Node* node, bool updateManager, bool beSilent )
{
	// Is a control panel for the node already created for the given node?
	EditorMetaData* editorMetaData = ( EditorMetaData* )node->GetMetaData();
	NodeControlPanel* nodeControlPanel = editorMetaData->GetNodeControlPanel();
	if( nodeControlPanel )
	{
		// Yes.  Make sure it's patched properly.
		nodeControlPanel->SetNode( node );
	}
	else
	{
		// No.  Try to create a control panel for the node.
		nodeControlPanel = NodeControlPanel::CreateForNode( this, node );
		if( !( nodeControlPanel || beSilent ) )
		{
			wxMessageBox( wxString::Format( wxT( "The editor is not aware of any node control panel interface supporting the node type \"%s\"." ), ( const char* )node->GetType().c_str() ), wxT( "Error" ), wxOK | wxCENTRE );
			return;
		}

		editorMetaData->SetNodeControlPanel( nodeControlPanel );
	}

	// Now go make sure that the node control panel is part of the UI.
	wxAuiPaneInfo& currentPaneInfo = auiManager.GetPane( nodeControlPanel );
	if( currentPaneInfo.window != 0 )
	{
		currentPaneInfo.Show();
		nodeControlPanel->SetFocus();
	}
	else
	{
		wxAuiPaneInfo paneInfo;
		paneInfo.Right().Dock().BestSize( 400, 400 );

		// Override default docking position/size if we have it remembered.
		wxString paneInfoString = editorMetaData->GetPaneInfoString();
		if( !paneInfoString.IsEmpty() )
			auiManager.LoadPaneInfo( paneInfoString, paneInfo );

		// Always override a few more things, most notably that of being shown.
		paneInfo.DestroyOnClose( false ).Caption( nodeControlPanel->GetCaption() ).Show().Name( wxT( "" ) );
		auiManager.AddPane( nodeControlPanel, paneInfo );
	}

	editorMetaData->SetShouldActivateNodeControlPanel( true );

	// Make sure the node control panel interface is up-to-date.
	nodeControlPanel->PushNodeDataToInterface();

	if( updateManager )
		auiManager.Update();
}

//=================================================================================
void DockingPanel::ActivateNodeControlPanelsForSubTree( Cornucopia::Node* node, bool beSilent )
{
	// Get a list of all the nodes in the given sub-tree.
	Cornucopia::Node::List nodeList;
	node->MakeNodeList( nodeList );

	// For each one, active it's control panel, if any.
	for( Cornucopia::Node::List::iterator iter = nodeList.begin(); iter != nodeList.end(); iter++ )
	{
		node = *iter;

		// For now, just skip base nodes, since I'm sure the user doesn't want to activate these.
		if( node->ThisClassInfo()->Type() != "Node" )
			ActivateNodeControlPanelForNode( node, false, beSilent );
	}

	auiManager.Update();
}

//=================================================================================
void DockingPanel::DeactivateNodeControlPanelForNode( Cornucopia::Node* node, bool updateManager, bool deletePanel /*= false*/ )
{
	SaveNodeControlPanelInfo( node );

	EditorMetaData* editorMetaData = ( EditorMetaData* )node->GetMetaData();
	NodeControlPanel* nodeControlPanel = editorMetaData->GetNodeControlPanel();
	if( nodeControlPanel )
	{
		editorMetaData->SetShouldActivateNodeControlPanel( false );

		auiManager.DetachPane( nodeControlPanel );
		nodeControlPanel->Hide();
		if( deletePanel )
		{
			// Hmmm.  Do I really know what I'm doing here?
			wxWindow* parent = nodeControlPanel->GetParent();
			parent->RemoveChild( nodeControlPanel );
			nodeControlPanel->Destroy();
			editorMetaData->SetNodeControlPanel(0);
		}

		if( updateManager )
			auiManager.Update();
	}
}

//=================================================================================
void DockingPanel::DeactivateNodeControlPanelsForSubTree( Cornucopia::Node* node, bool updateManager, bool deletePanel /*= false*/ )
{
	// Get a list of all the nodes in the given sub-tree.
	Cornucopia::Node::List nodeList;
	node->MakeNodeList( nodeList );

	// For each one, remove and destroy its active node control panel, if any.
	for( Cornucopia::Node::List::iterator iter = nodeList.begin(); iter != nodeList.end(); iter++ )
	{
		node = *iter;
		DeactivateNodeControlPanelForNode( node, false, deletePanel );
	}

	if( updateManager )
		auiManager.Update();
}

//=================================================================================
/*virtual*/ void DockingPanel::HighlightNodeControlPanel( NodeControlPanel* highlightNodeControlPanel )
{
	Panel::HighlightNodeControlPanel( highlightNodeControlPanel );
	
	if( highlightNodeControlPanel )
	{
		Cornucopia::Node* node = highlightNodeControlPanel->GetNode();
		HighlightNodeItem( node );
	}
	
	auiManager.Update();
}

//=================================================================================
void DockingPanel::HighlightNodeItem( Cornucopia::Node* node )
{
	wxDataViewItem item( node );
	dataViewCtrl->Select( item );
}

//=================================================================================
DockingPanel::DockArt::DockArt( void )
{
}

//=================================================================================
/*virtual*/ DockingPanel::DockArt::~DockArt( void )
{
}

//=================================================================================
/*virtual*/ void DockingPanel::DockArt::DrawCaption( wxDC& dc, wxWindow* window, const wxString& text, const wxRect& rect, wxAuiPaneInfo& pane )
{
	NodeControlPanel* nodeControlPanel = wxDynamicCast( pane.window, NodeControlPanel );
	if( nodeControlPanel )
	{
		wxColour color = wxSystemSettings::GetColour( wxSYS_COLOUR_INACTIVECAPTION );
		if( nodeControlPanel->GetHighlighted() )
			color = wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT );

		SetColour( wxAUI_DOCKART_ACTIVE_CAPTION_COLOUR, color );
		SetColour( wxAUI_DOCKART_INACTIVE_CAPTION_COLOUR, color );
	}

	SetMetric( wxAUI_DOCKART_GRADIENT_TYPE, wxAUI_GRADIENT_NONE );
	
	wxAuiDefaultDockArt::DrawCaption( dc, window, text, rect, pane );
}

//=================================================================================
wxAuiManager* DockingPanel::GetAuiManager( void )
{
	return &auiManager;
}

//=================================================================================
DataViewCtrl* DockingPanel::GetDataViewControl( void )
{
	return dataViewCtrl;
}

//=================================================================================
/*virtual*/ bool DockingPanel::FindNext( const wxString& searchCriteria )
{
	// Nothing is found in an empty tree.
	Cornucopia::Node* root = tree->GetRoot();
	if( !root )
		return false;

	// Create the list of nodes we'll search through.  A linear search is fine for now.
	Cornucopia::Node::List nodeList;
	root->MakeNodeList( nodeList );
	Cornucopia::Node::List::iterator iter = nodeList.begin();

	// If an item is currently selected in the data-view control, begin
	// our search just after this item.  Otherwise, we'll begin our search
	// from the beginning.
	wxDataViewItem selectedItem = dataViewCtrl->GetSelection();
	if( selectedItem.IsOk() )
	{
		while( iter != nodeList.end() && *iter != selectedItem.GetID() )
			iter++;
		if( iter == nodeList.end() )
			return false;		// This should never happen.
		iter++;		// Begin the search just after the currently selected item.
	}

	// Go perform the search.
	bool isWild = wxIsWild( searchCriteria );
	while( iter != nodeList.end() )
	{
		Cornucopia::Node* node = *iter;
		wxString nodeName = ( const char* )node->GetName().c_str();
		if( ( isWild && wxMatchWild( searchCriteria, nodeName, false ) ) || nodeName.CmpNoCase( searchCriteria ) == 0 )
			break;
		iter++;
	}

	// If we didn't find it, indicate that to the caller.
	if( iter == nodeList.end() )
		return false;

	// We found it!  Select it for the user.
	selectedItem.m_pItem = *iter;
	wxDataViewItemArray selectionArray;
	selectionArray.Add( selectedItem );
	dataViewCtrl->SetSelections( selectionArray );
	dataViewCtrl->SetCurrentItem( selectedItem );

	// TODO: If the found item is scrolled out of the parent window, how do we auto-scroll it into view?

	// Return success in finding the node.
	return true;
}

// DockingPanel.cpp