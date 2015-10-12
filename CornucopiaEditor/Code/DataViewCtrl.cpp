// DataViewCtrl.cpp

#include "CornucopiaEditor.h"

using namespace CornucopiaEditor;

int DataViewCtrl::shownColumnFlags = ( 1 << DataViewModel::COL_NAME ) | ( 1 << DataViewModel::COL_VALUE );

//=================================================================================
BEGIN_EVENT_TABLE( DataViewCtrl, wxDataViewCtrl )

	EVT_DATAVIEW_ITEM_ACTIVATED( wxID_ANY, OnItemActivated )
	EVT_DATAVIEW_ITEM_COLLAPSED( wxID_ANY, OnItemCollapsed )
	EVT_DATAVIEW_ITEM_EXPANDED( wxID_ANY, OnItemExpanded )
	EVT_DATAVIEW_ITEM_CONTEXT_MENU( wxID_ANY, OnContextMenu )

	EVT_DATAVIEW_ITEM_START_EDITING( wxID_ANY, OnStartEditing )
    EVT_DATAVIEW_ITEM_EDITING_STARTED( wxID_ANY, OnEditingStarted )
    EVT_DATAVIEW_ITEM_EDITING_DONE( wxID_ANY, OnEditingDone )

	EVT_DATAVIEW_SELECTION_CHANGED( wxID_ANY, OnSelectionChanged )

	EVT_DATAVIEW_COLUMN_REORDERED( wxID_ANY, OnColumnReordered )
	EVT_DATAVIEW_COLUMN_HEADER_CLICK( wxID_ANY, OnColumnHeaderClick )

	EVT_MENU( ID_ContextMenu_Create, OnContextMenuCreate )
	EVT_MENU( ID_ContextMenu_ChangeType, OnContextMenuChangeType )
	EVT_MENU( ID_ContextMenu_Cut, OnContextMenuCut )
	EVT_MENU( ID_ContextMenu_DeepCopy, OnContextMenuDeepCopy )
	EVT_MENU( ID_ContextMenu_ShallowCopy, OnContextMenuShallowCopy )
	EVT_MENU( ID_ContextMenu_PasteUnder, OnContextMenuPasteUnder )
	EVT_MENU( ID_ContextMenu_PasteOver, OnContextMenuPasteOver )
	EVT_MENU( ID_ContextMenu_Delete, OnContextMenuDelete )

	EVT_MENU( ID_ContextMenu_MoveForward, OnContextMenuMoveForward )
	EVT_MENU( ID_ContextMenu_MoveBackward, OnContextMenuMoveBackward )

	EVT_MENU( ID_ContextMenu_EditName, OnContextMenuEditName )
	EVT_MENU( ID_ContextMenu_EditType, OnContextMenuEditType )
	EVT_MENU( ID_ContextMenu_EditValue, OnContextMenuEditValue )

	EVT_MENU( ID_ContextMenu_EditShortExplanation, OnContextMenuEditShortExplanation )
	EVT_MENU( ID_ContextMenu_EditLongExplanation, OnContextMenuEditLongExplanation )

	EVT_MENU( ID_ContextMenu_ToggleNameColumn, OnContextMenuToggleColumn )
	EVT_MENU( ID_ContextMenu_ToggleTypeColumn, OnContextMenuToggleColumn )
	EVT_MENU( ID_ContextMenu_ToggleValueColumn, OnContextMenuToggleColumn )
	EVT_MENU( ID_ContextMenu_ToggleFeedbackColumn, OnContextMenuToggleColumn )

	EVT_MENU( ID_ContextMenu_ToggleVisibility, OnContextMenuToggleVisibility )

	EVT_MENU( ID_ContextMenu_ActivateNodeControlPanel, OnActivateNodeControlPanel )
	EVT_MENU( ID_ContextMenu_ActivateSubTreeControlPanels, OnActivateSubTreeControlPanels )

	EVT_MENU( ID_ContextMenu_DeactivateNodeControlPanel, OnDeactivateNodeControlPanel )
	EVT_MENU( ID_ContextMenu_DeactivateSubTreeControlPanels, OnDeactivateSubTreeControlPanels )

	EVT_MENU( ID_ContextMenu_FollowFileReference, OnFollowFileReference )

	EVT_MENU( ID_ContextMenu_RestoreDefaultValue, OnContextMenuRestoreDefaultValue )

END_EVENT_TABLE()

//=================================================================================
wxIMPLEMENT_DYNAMIC_CLASS( DataViewCtrl, wxDataViewCtrl )

//=================================================================================
DataViewCtrl::DataViewCtrl( wxWindow* parent ) : wxDataViewCtrl( parent, wxID_ANY )
{
	const int acceleratorTableEntryListSize = 11;
	wxAcceleratorEntry acceleratorTableEntryList[ acceleratorTableEntryListSize ];
	acceleratorTableEntryList[0].Set( wxACCEL_CTRL, int( 'a' ), ID_ContextMenu_ActivateSubTreeControlPanels );
	acceleratorTableEntryList[1].Set( wxACCEL_ALT, int( 'a' ), ID_ContextMenu_ActivateNodeControlPanel );
	acceleratorTableEntryList[2].Set( wxACCEL_CTRL, int( 'd' ), ID_ContextMenu_DeactivateSubTreeControlPanels );
	acceleratorTableEntryList[3].Set( wxACCEL_ALT, int( 'd' ), ID_ContextMenu_DeactivateNodeControlPanel );
	acceleratorTableEntryList[4].Set( wxACCEL_CTRL, int( 'x' ), ID_ContextMenu_Cut );
	acceleratorTableEntryList[5].Set( wxACCEL_CTRL, int( 'c' ), ID_ContextMenu_DeepCopy );
	acceleratorTableEntryList[6].Set( wxACCEL_ALT, int( 'c' ), ID_ContextMenu_ShallowCopy );
	acceleratorTableEntryList[7].Set( wxACCEL_CTRL, int( 'v' ), ID_ContextMenu_PasteUnder );
	acceleratorTableEntryList[8].Set( wxACCEL_ALT, int( 'v' ), ID_ContextMenu_PasteOver );
	acceleratorTableEntryList[9].Set( wxACCEL_CTRL, WXK_PAGEUP, ID_ContextMenu_MoveForward );
	acceleratorTableEntryList[10].Set( wxACCEL_CTRL, WXK_PAGEDOWN, ID_ContextMenu_MoveBackward );
	wxAcceleratorTable acceleratorTable( acceleratorTableEntryListSize, acceleratorTableEntryList );
	SetAcceleratorTable( acceleratorTable );

	Bind( wxEVT_HELP, &DataViewCtrl::OnContextSensativeHelp, this );
}

//=================================================================================
/*virtual*/ DataViewCtrl::~DataViewCtrl( void )
{
}

//=================================================================================
void DataViewCtrl::RebuildColumns( void )
{
	// Note that we can't just delete all columns and then add the columns we want here.
	// This is because some columns are created and managed by plugins, and we have
	// no knowledge of that here.  So here we just add and remove the columns we care about.
	for( int col = DataViewModel::COL_NAME; col < DataViewModel::COL_UNUSED; col++ )
	{
		wxDataViewColumn* dataViewColumn = FindModelColumn( col );
		if( shownColumnFlags & ( 1 << col ) )
		{
			if( !dataViewColumn )
			{
				dataViewColumn = new wxDataViewColumn( ColModelIndexToColName( col ),
									new wxDataViewTextRenderer( wxT( "string" ), wxDATAVIEW_CELL_EDITABLE ),
									col, 200, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_REORDERABLE );
				if( col < ( signed )GetColumnCount() )
					InsertColumn( col, dataViewColumn );
				else
					AppendColumn( dataViewColumn );
			}
		}
		else
		{
			if( dataViewColumn )
				DeleteColumn( dataViewColumn );
		}
	}

	// Let the first column, if any, always be the expander column.
	if( GetColumnCount() > 0 )
	{
		wxDataViewColumn* dataViewColumn = GetColumn(0);
		SetExpanderColumn( dataViewColumn );
	}
}

//=================================================================================
int DataViewCtrl::GetUnusedModelColumn( void )
{
	int modelCol = DataViewModel::COL_UNUSED;
	while( IsModelColumnUsed( modelCol ) )
		modelCol++;
	return modelCol;
}

//=================================================================================
bool DataViewCtrl::IsModelColumnUsed( int modelCol )
{
	wxDataViewColumn* dataViewColumn = FindModelColumn( modelCol );
	if( dataViewColumn )
		return true;
	return false;
}

//=================================================================================
wxDataViewColumn* DataViewCtrl::FindModelColumn( int modelCol )
{
	for( unsigned int index = 0; index < GetColumnCount(); index++ )
	{
		wxDataViewColumn* dataViewColumn = GetColumn( index );
		if( dataViewColumn->GetModelColumn() == modelCol )
			return dataViewColumn;
	}
	return 0;
}

//=================================================================================
/*static*/ wxString DataViewCtrl::ColModelIndexToColName( int modelCol )
{
	switch( modelCol )
	{
		case DataViewModel::COL_NAME:		return wxT( "Name" );
		case DataViewModel::COL_TYPE:		return wxT( "Type" );
		case DataViewModel::COL_VALUE:		return wxT( "Value" );
		case DataViewModel::COL_FEEDBACK:	return wxT( "Feedback" );
	}

	return wxT( "???" );
}

//=================================================================================
void DataViewCtrl::OnColumnReordered( wxDataViewEvent& event )
{
	// Why doesn't wxWidgets send me this event when I think it should?
}

//=================================================================================
void DataViewCtrl::OnColumnHeaderClick( wxDataViewEvent& event )
{
	wxDataViewColumn* dataViewColumn = GetColumn( event.GetColumn() );
	if( dataViewColumn )
		SetExpanderColumn( dataViewColumn );
}

//=================================================================================
void DataViewCtrl::OnContextSensativeHelp( wxHelpEvent& event )
{
	wxPoint mousePoint = event.GetPosition();
	mousePoint = ScreenToClient( mousePoint );
	mousePoint.y -= 23;		// HACK: Try to account for column header height.
	wxDataViewItem item;
	wxDataViewColumn* column = 0;
	HitTest( mousePoint, item, column );
	if( item.IsOk() )
	{
		Cornucopia::Node* node = ( Cornucopia::Node* )item.m_pItem;
		DockingPanel* dockingPanel = GetAssociatedPanel();
		dockingPanel->DisplayContextSensativeHelpForNode( node );
	}
}

//=================================================================================
DockingPanel* DataViewCtrl::GetAssociatedPanel( void )
{
	DockingPanel* dockingPanel = 0;
	
	wxWindow* parent = GetParent();
	while( parent )
	{
		dockingPanel = wxDynamicCast( parent, DockingPanel );
		if( dockingPanel )
			break;

		parent = parent->GetParent();
	}

	return dockingPanel;
}

//=================================================================================
void DataViewCtrl::OnSelectionChanged( wxDataViewEvent& event )
{
	wxDataViewItem item = event.GetItem();
	if( item.IsOk() )
	{
		// Using the status bar, keep user informed about what nodes are for.
		Cornucopia::Node* node = ( Cornucopia::Node* )item.m_pItem;
		wxString shortExplanation = ( const char* )node->GetExplanation( Cornucopia::Node::SHORT_EXPLANATION ).c_str();
		wxStatusBar* statusBar = wxGetApp().GetFrame()->GetStatusBar();
		if( statusBar )
		{
			if( !shortExplanation.IsEmpty() )
				statusBar->SetStatusText( shortExplanation );
			else
				statusBar->SetStatusText( "Node has no short explanation set." );
		}

		// Highlight this node's associated panel, if any.
		EditorMetaData* editorMetaData = ( EditorMetaData* )node->GetMetaData();
		NodeControlPanel* nodeControlPanel = editorMetaData->GetNodeControlPanel();
		DockingPanel* dockingPanel = GetAssociatedPanel();
		dockingPanel->HighlightNodeControlPanel( nodeControlPanel );
	}
}

//=================================================================================
void DataViewCtrl::OnItemActivated( wxDataViewEvent& event )
{
	wxDataViewItem item = event.GetItem();
	Cornucopia::Node* node = ( Cornucopia::Node* )item.GetID();

	wxMouseState mouseState = wxGetMouseState();
	if( mouseState.ControlDown() )
	{
		// Bring up documentation on the activated node.
		wxString initialPage = wxString( wxGetApp().GetResourceManager()->FormulateFullPath( "Documentation/Nodes/" ) ) + wxString( ( const char* )node->GetType().c_str() ) + wxString( ".html" );
		wxGetApp().GetFrame()->ActivateDocumentationPanel( initialPage );
	}
	else
	{
		// Activate a node control panel interface for the given node.
		DockingPanel* dockingPanel = GetAssociatedPanel();
		dockingPanel->ActivateNodeControlPanelForNode( node, false, false );

		// Make sure that newly activated panels are also highlighted.
		EditorMetaData* editorMetaData = ( EditorMetaData* )node->GetMetaData();
		NodeControlPanel* nodeControlPanel = editorMetaData->GetNodeControlPanel();
		if( nodeControlPanel && !nodeControlPanel->GetHighlighted() )
			dockingPanel->HighlightNodeControlPanel( nodeControlPanel );
		else
			dockingPanel->GetAuiManager()->Update();
	}
}

//=================================================================================
void DataViewCtrl::OnItemCollapsed( wxDataViewEvent& event )
{
	wxDataViewItem item = event.GetItem();
	Cornucopia::Node* node = ( Cornucopia::Node* )item.GetID();
	EditorMetaData* editorMetaData = ( EditorMetaData* )node->GetMetaData();
	editorMetaData->SetExpanded( false );
}

//=================================================================================
void DataViewCtrl::OnItemExpanded( wxDataViewEvent& event )
{
	wxDataViewItem item = event.GetItem();
	Cornucopia::Node* node = ( Cornucopia::Node* )item.GetID();
	EditorMetaData* editorMetaData = ( EditorMetaData* )node->GetMetaData();
	editorMetaData->SetExpanded( true );
}

//=================================================================================
void DataViewCtrl::OnContextMenu( wxDataViewEvent& event )
{
	wxDataViewItem contextItem = event.GetItem();

	wxMenu* editSubMenu = new wxMenu();
	editSubMenu->Append( ID_ContextMenu_EditName, wxT( "Name" ) );
	editSubMenu->Append( ID_ContextMenu_EditType, wxT( "Type" ) );
	editSubMenu->Append( ID_ContextMenu_EditValue, wxT( "Value" ) );
	editSubMenu->AppendSeparator();
	editSubMenu->Append( ID_ContextMenu_EditShortExplanation, wxT( "Short Explanation" ) );
	editSubMenu->Append( ID_ContextMenu_EditLongExplanation, wxT( "Long Explanation" ) );

	wxMenu* columnsSubMenu = new wxMenu();
	columnsSubMenu->Append( ID_ContextMenu_ToggleNameColumn, wxT( "Name" ), wxEmptyString, wxITEM_CHECK );
	columnsSubMenu->Append( ID_ContextMenu_ToggleTypeColumn, wxT( "Type" ), wxEmptyString, wxITEM_CHECK );
	columnsSubMenu->Append( ID_ContextMenu_ToggleValueColumn, wxT( "Value" ), wxEmptyString, wxITEM_CHECK );
	columnsSubMenu->Append( ID_ContextMenu_ToggleFeedbackColumn, wxT( "Feedback" ), wxEmptyString, wxITEM_CHECK );

	wxMenu* activateSubMenu = new wxMenu();
	activateSubMenu->Append( ID_ContextMenu_ActivateNodeControlPanel, wxT( "Activate Node Control Panel\tAlt-A" ), wxT( "Bring up the node control panel interface for the node." ) );
	activateSubMenu->Append( ID_ContextMenu_ActivateSubTreeControlPanels, wxT( "Activate Sub-Tree Control Panels\tCtrl-A" ), wxT( "Bring up the node control panel interfaces for all nodes at and under the given node." ) );

	wxMenu* deactivateSubMenu = new wxMenu();
	deactivateSubMenu->Append( ID_ContextMenu_DeactivateNodeControlPanel, wxT( "Deactivate Node Control Panel\tAlt-D" ), wxT( "Bring down the node control panel interface for the node." ) );
	deactivateSubMenu->Append( ID_ContextMenu_DeactivateSubTreeControlPanels, wxT( "Deactivate Sub-Tree Control Panels\tCtrl-D" ), wxT( "Bring down the node control panel interfaces for all nodes at and under the given node." ) );

	wxMenu contextMenu;
	
	if( contextItem.IsOk() )
	{
		Cornucopia::Node* node = ( Cornucopia::Node* )contextItem.GetID();
		wxString nodeType = ( const char* )node->GetType().c_str();
		if( nodeType == wxT( "FileReferenceNode" ) )
		{
			wxMenuItem* openMenuItem = new wxMenuItem( &contextMenu, ID_ContextMenu_FollowFileReference, wxT( "Open" ), wxT( "Open the file referenced by this node in this editor or some other known program." ) );
			openMenuItem->SetBitmap( wxArtProvider::GetBitmap( wxART_FILE_OPEN ) );
			contextMenu.Append( openMenuItem );
			contextMenu.AppendSeparator();
		}
	}

	wxMenuItem* createMenuItem = new wxMenuItem( &contextMenu, ID_ContextMenu_Create, wxT( "Create" ), wxT( "Create a new node as a child of the selected node or as the root of a new tree." ) );
	wxMenuItem* changeTypeMenuItem = new wxMenuItem( &contextMenu, ID_ContextMenu_ChangeType, wxT( "Change Type" ), wxT( "Change the type of this node." ) );
	createMenuItem->SetBitmap( wxArtProvider::GetBitmap( wxART_NEW ) );
	changeTypeMenuItem->SetBitmap( wxArtProvider::GetBitmap( wxART_NEW ) );
	contextMenu.Append( createMenuItem );
	contextMenu.Append( changeTypeMenuItem );
	
	contextMenu.AppendSeparator();
	wxMenuItem* cutMenuItem = new wxMenuItem( &contextMenu, ID_ContextMenu_Cut, wxT( "Cut\tCtrl-X" ), wxT( "Prepare to cut the selected sub-tree and paste it somewhere else." ) );
	wxMenuItem* deepCopyMenuItem = new wxMenuItem( &contextMenu, ID_ContextMenu_DeepCopy, wxT( "Deep Copy\tCtrl-C" ), wxT( "Prepare to copy the selected sub-tree and paste it somewhere else." ) );
	wxMenuItem* shallowCopyMenuItem = new wxMenuItem( &contextMenu, ID_ContextMenu_ShallowCopy, wxT( "Shallow Copy\tAlt-C" ), wxT( "Prepare to copy the selected node and paste it somewhere else." ) );
	wxMenuItem* pasteUnderMenuItem = new wxMenuItem( &contextMenu, ID_ContextMenu_PasteUnder, wxT( "Paste Under\tCtrl-V" ), wxT( "Follow through with a previously planned cut or paste operation by pasting under the selected node." ) );
	wxMenuItem* pasteOverMenuItem = new wxMenuItem( &contextMenu, ID_ContextMenu_PasteOver, wxT( "Paste Over\tAlt-V" ), wxT( "Follow through with a previously planned cut or past operation by pasting over the selected node." ) );
	cutMenuItem->SetBitmap( wxArtProvider::GetBitmap( wxART_CUT ) );
	deepCopyMenuItem->SetBitmap( wxArtProvider::GetBitmap( wxART_COPY ) );
	shallowCopyMenuItem->SetBitmap( wxArtProvider::GetBitmap( wxART_COPY ) );
	pasteUnderMenuItem->SetBitmap( wxArtProvider::GetBitmap( wxART_PASTE ) );
	pasteOverMenuItem->SetBitmap( wxArtProvider::GetBitmap( wxART_PASTE ) );
	contextMenu.Append( cutMenuItem );
	contextMenu.Append( deepCopyMenuItem );
	contextMenu.Append( shallowCopyMenuItem );
	contextMenu.Append( pasteUnderMenuItem );
	contextMenu.Append( pasteOverMenuItem );
	
	contextMenu.AppendSeparator();
	wxMenuItem* moveForwardItem = new wxMenuItem( &contextMenu, ID_ContextMenu_MoveForward, wxT( "Move Forward\tCtrl-PgUp" ), wxT( "Move the node forward in the child list order." ) );
	wxMenuItem* moveBackwardItem = new wxMenuItem( &contextMenu, ID_ContextMenu_MoveBackward, wxT( "Move Backward\tCtrl-PgDn" ), wxT( "Move the node backward in the child list order." ) );
	moveForwardItem->SetBitmap( wxArtProvider::GetBitmap( wxART_GO_FORWARD ) );
	moveBackwardItem->SetBitmap( wxArtProvider::GetBitmap( wxART_GO_BACK ) );
	contextMenu.Append( moveForwardItem );
	contextMenu.Append( moveBackwardItem );

	moveForwardItem->Enable( false );
	moveBackwardItem->Enable( false );
	if( contextItem.IsOk() )
	{
		Cornucopia::Node* node = ( Cornucopia::Node* )contextItem.GetID();
		Cornucopia::Node::List::const_iterator locationIter;
		if( node->ChildLocation( locationIter ) )
		{
			if( node->GetParent() )
			{
				const Cornucopia::Node::List* children = node->GetParent()->GetChildren();
				moveBackwardItem->Enable( locationIter != children->begin() );
				moveForwardItem->Enable( ++locationIter != children->end() );
			}
		}
	}

	if( contextItem.IsOk() )
	{
		contextMenu.AppendSeparator();
		wxMenuItem* visibilityItem = contextMenu.AppendCheckItem( ID_ContextMenu_ToggleVisibility, wxT( "Visible" ), wxT( "Toggle the visibility of the node in other tree interfaces." ) );
		Cornucopia::Node* node = ( Cornucopia::Node* )contextItem.GetID();
		EditorMetaData* editorMetaData = ( EditorMetaData* )node->GetMetaData();
		visibilityItem->Check( editorMetaData->GetVisibility() );
	}

	contextMenu.AppendSeparator();
	contextMenu.AppendSubMenu( editSubMenu, wxT( "Edit" ), wxT( "Edit the name, type or value of the selected node." ) );
	contextMenu.AppendSubMenu( columnsSubMenu, wxT( "Columns" ), wxT( "Toggle the visibility of columns." ) );

	if( contextItem.IsOk() )
	{
		Cornucopia::Node* node = ( Cornucopia::Node* )contextItem.GetID();
		EditorMetaData* editorMetaData = ( EditorMetaData* )node->GetMetaData();
		NodeControlPanel* nodeControlPanel = editorMetaData->GetNodeControlPanel();
		if( nodeControlPanel )
		{
			// The node control panel should skip the menu command event if it does not handle it.
			wxMenu* nodeMenu = nodeControlPanel->CreateMenu( contextMenu.GetEventHandler() );
			if( nodeMenu )
			{
				wxString nodeType = ( const char* )node->GetType().c_str();
				contextMenu.AppendSubMenu( nodeMenu, nodeType );
			}
		}
	}

	contextMenu.AppendSeparator();
	contextMenu.AppendSubMenu( activateSubMenu, wxT( "Activate" ), wxT( "Activate one or more node control panels for various nodes." ) );
	contextMenu.AppendSubMenu( deactivateSubMenu, wxT( "Deactivate" ), wxT( "Deactivate one or more node control panels for various nodes." ) );

	if( contextItem.IsOk() )
	{
		contextMenu.AppendSeparator();
		contextMenu.Append( ID_ContextMenu_RestoreDefaultValue, wxT( "Restore Default Value" ), wxT( "Reset the selected node to its default value." ) );
	}

	contextMenu.AppendSeparator();
	wxMenuItem* deleteMenuItem = new wxMenuItem( &contextMenu, ID_ContextMenu_Delete, wxT( "Delete" ), wxT( "Delete the selected node and all of its children." ) );
	deleteMenuItem->SetBitmap( wxArtProvider::GetBitmap( wxART_DELETE ) );
	contextMenu.Append( deleteMenuItem );

	contextMenu.FindItem( ID_ContextMenu_ChangeType )->Enable( contextItem.IsOk() );
	contextMenu.FindItem( ID_ContextMenu_EditName )->Enable( contextItem.IsOk() );
	contextMenu.FindItem( ID_ContextMenu_EditType )->Enable( contextItem.IsOk() );
	contextMenu.FindItem( ID_ContextMenu_EditValue )->Enable( contextItem.IsOk() );
	contextMenu.FindItem( ID_ContextMenu_Cut )->Enable( contextItem.IsOk() );
	contextMenu.FindItem( ID_ContextMenu_DeepCopy )->Enable( contextItem.IsOk() );
	contextMenu.FindItem( ID_ContextMenu_ShallowCopy )->Enable( contextItem.IsOk() );
	contextMenu.FindItem( ID_ContextMenu_Delete )->Enable( contextItem.IsOk() );
	contextMenu.FindItem( ID_ContextMenu_ActivateNodeControlPanel )->Enable( contextItem.IsOk() );
	contextMenu.FindItem( ID_ContextMenu_ActivateSubTreeControlPanels )->Enable( contextItem.IsOk() );
	contextMenu.FindItem( ID_ContextMenu_DeactivateNodeControlPanel )->Enable( contextItem.IsOk() );
	contextMenu.FindItem( ID_ContextMenu_DeactivateSubTreeControlPanels )->Enable( contextItem.IsOk() );
	contextMenu.FindItem( ID_ContextMenu_EditShortExplanation )->Enable( contextItem.IsOk() );
	contextMenu.FindItem( ID_ContextMenu_EditLongExplanation )->Enable( contextItem.IsOk() );

	contextMenu.FindItem( ID_ContextMenu_ToggleNameColumn )->Check( ( shownColumnFlags & ( 1 << DataViewModel::COL_NAME ) ) ? true : false );
	contextMenu.FindItem( ID_ContextMenu_ToggleTypeColumn )->Check( ( shownColumnFlags & ( 1 << DataViewModel::COL_TYPE ) ) ? true : false );
	contextMenu.FindItem( ID_ContextMenu_ToggleValueColumn )->Check( ( shownColumnFlags & ( 1 << DataViewModel::COL_VALUE ) ) ? true : false );
	contextMenu.FindItem( ID_ContextMenu_ToggleFeedbackColumn )->Check( ( shownColumnFlags & ( 1 << DataViewModel::COL_FEEDBACK ) ) ? true : false );

	wxDataFormat dataFormat( DataObject::GetFormatString() );
	bool pasteAvailable = wxTheClipboard->IsSupported( dataFormat );
	contextMenu.FindItem( ID_ContextMenu_PasteUnder )->Enable( pasteAvailable );
	contextMenu.FindItem( ID_ContextMenu_PasteOver )->Enable( pasteAvailable );

	if( contextItem.IsOk() )
	{
		Cornucopia::Node* node = ( Cornucopia::Node* )contextItem.m_pItem;
		contextMenu.FindItem( ID_ContextMenu_RestoreDefaultValue )->Enable( node->DefaultValueCanBeRestored() );
	}

	// Pop it up at the cursor.
	wxPoint point = event.GetPosition();
	PopupMenu( &contextMenu, point );
}

//=================================================================================
void DataViewCtrl::OnFollowFileReference( wxCommandEvent& event )
{
	wxDataViewItem selectedItem = GetSelection();
	if( selectedItem.IsOk() )
	{
		Cornucopia::Node* node = ( Cornucopia::Node* )selectedItem.GetID();
		Cornucopia::FileReferenceNode* fileRefNode = node->Cast< Cornucopia::FileReferenceNode >();
		if( fileRefNode )
		{
			Cornucopia::Path filePath;
			if( fileRefNode->GetFilePath( filePath ) )
			{
				// TODO: If we can't open it, ask a plugin to open it in the appropriate application.
				Frame* frame = wxGetApp().GetFrame();
				frame->OpenTree( wxString( ( const char* )filePath ) );
			}
		}
	}
}

//=================================================================================
void DataViewCtrl::OnActivateNodeControlPanel( wxCommandEvent& event )
{
	wxDataViewItem selectedItem = GetSelection();
	if( selectedItem.IsOk() )
	{
		Cornucopia::Node* node = ( Cornucopia::Node* )selectedItem.GetID();
		DockingPanel* dockingPanel = GetAssociatedPanel();
		dockingPanel->ActivateNodeControlPanelForNode( node, true, false );
	}
}

//=================================================================================
void DataViewCtrl::OnActivateSubTreeControlPanels( wxCommandEvent& event )
{
	wxDataViewItem selectedItem = GetSelection();
	if( selectedItem.IsOk() )
	{
		Cornucopia::Node* node = ( Cornucopia::Node* )selectedItem.GetID();
		DockingPanel* dockingPanel = GetAssociatedPanel();
		dockingPanel->ActivateNodeControlPanelsForSubTree( node, false );
	}
}

//=================================================================================
void DataViewCtrl::OnDeactivateNodeControlPanel( wxCommandEvent& event )
{
	wxDataViewItem selectedItem = GetSelection();
	if( selectedItem.IsOk() )
	{
		Cornucopia::Node* node = ( Cornucopia::Node* )selectedItem.GetID();
		DockingPanel* dockingPanel = GetAssociatedPanel();
		dockingPanel->DeactivateNodeControlPanelForNode( node, true, true );
	}
}

//=================================================================================
void DataViewCtrl::OnDeactivateSubTreeControlPanels( wxCommandEvent& event )
{
	wxDataViewItem selectedItem = GetSelection();
	if( selectedItem.IsOk() )
	{
		Cornucopia::Node* node = ( Cornucopia::Node* )selectedItem.GetID();
		DockingPanel* dockingPanel = GetAssociatedPanel();
		dockingPanel->DeactivateNodeControlPanelsForSubTree( node, true, true );
	}
}

//=================================================================================
/*static*/ bool DataViewCtrl::QueryForNodeType( const wxString& caption, wxString& nodeType )
{
	// Query the core library for all registered node types.
	std::list< std::string > nodeTypeList;
	wxGetApp().GetContext().MakeListOfAllRegisteredNodeTypes( nodeTypeList, true );
	if( nodeTypeList.size() == 0 )
	{
		wxMessageBox( wxT( "There are no node types registered with the Cornucopia system!" ), wxT( "Error" ), wxOK | wxCENTRE );
		return false;
	}

	// Do a necessary and unfortunate container conversion.
	int index = 0;
	wxScopedArray< wxString > nodeTypeArray( new wxString[ nodeTypeList.size() ] );
	for( std::list< std::string >::iterator iter = nodeTypeList.begin(); iter != nodeTypeList.end(); iter++ )
		nodeTypeArray[ index++ ] = *iter;

	// Present all node types in a list to the user to choose from.
	// It should be in alphabetical order so that it's not horrible if long.
	wxSingleChoiceDialog singleChoiceDialog( wxGetApp().GetFrame(), wxT( "Please choose from the following list of known node types." ), caption, nodeTypeList.size(), nodeTypeArray.get() );
	if( wxID_OK != singleChoiceDialog.ShowModal() )
		return false;

	// Get and return the user's choice.
	index = singleChoiceDialog.GetSelection();
	nodeType = nodeTypeArray[ index ];
	return true;
}

//=================================================================================
void DataViewCtrl::OnContextMenuCreate( wxCommandEvent& event )
{
	// If we have no parent for the node to be created, then we're creating
	// a root node.  But we can't do that if the tree already has a root.
	DockingPanel* dockingPanel = GetAssociatedPanel();
	Tree* tree = dockingPanel->GetTree();
	wxDataViewItem selectedItem = GetSelection();
	if( !selectedItem.IsOk() && tree->GetRoot() )
		return;

	// Ask the user for the desired not type to instantiate.
	wxString nodeType;
	if( !QueryForNodeType( wxT( "Create" ), nodeType ) )
		return;
	
	// Try to create the node type.  This will also create the node's meta-data.
	wxScopedPtr< Cornucopia::Node > node( wxGetApp().GetContext().CreateNode( ( const char* )nodeType.c_str() ) );
	if( !node )
	{
		wxGetApp().CornucopiaErrorMessageBox();
		return;
	}

	// Try to add it to the current tree or lack there-of.
	Cornucopia::Node* parent = ( Cornucopia::Node* )selectedItem.GetID();
	if( parent )
	{
		if( !node->AttachToParent( parent, wxGetApp().GetContext() ) )
		{
			wxGetApp().CornucopiaErrorMessageBox();
			return;
		}
	}
	else
	{
		if( !tree->SetRoot( node.get() ) )
		{
			wxMessageBox( wxT( "Failed to set the newly created node as the root node of the panel's tree.  Does it already have a root?" ), wxT( "Error" ), wxOK | wxCENTRE );
			return;
		}
	}

	// Tell the tree control we added a new item.
	// Notice that we prevent the scope delete here too!
	wxDataViewModel* dataViewModel = GetModel();
	wxDataViewItem parentItem( parent );
	wxDataViewItem newItem( node.release() );
	dataViewModel->ItemAdded( parentItem, newItem );
	Expand( parentItem );
	Select( newItem );

	// Make sure we update the modification status of the panel.
	tree->SetModified( true );
	NoteBook* noteBook = wxGetApp().GetFrame()->GetNoteBook();
	noteBook->UpdateTabTitle( dockingPanel );

	// Add to the undo-redo system.
	dockingPanel->TakeSnapShot();
}

//=================================================================================
void DataViewCtrl::OnContextMenuChangeType( wxCommandEvent& event )
{
	wxDataViewItem selectedItem = GetSelection();
	if( !selectedItem.IsOk() )
		return;

	// Ask the user for the desired not type to change to.
	wxString nodeType;
	if( !QueryForNodeType( wxT( "Change Type" ), nodeType ) )
		return;
	
	// If the type didn't change, we're done!
	wxScopedPtr< Cornucopia::Node > oldNode( ( Cornucopia::Node* )selectedItem.GetID() );
	if( wxString( oldNode->GetType().c_str() ) == nodeType )
	{
		oldNode.release();
		return;
	}

	// Try to create the node type.  This will also create the node's meta-data.
	wxScopedPtr< Cornucopia::Node > newNode( wxGetApp().GetContext().CreateNode( ( const char* )nodeType.c_str() ) );
	if( !newNode )
	{
		wxGetApp().CornucopiaErrorMessageBox();
		return;
	}

	// Give the new node the same name as the old node, unless it was named after the type.
	std::string name = oldNode->GetName();
	if( name == oldNode->GetType() )
		name = newNode->GetType();
	newNode->SetName( name );

	// Save the attchment point of the old node.
	Cornucopia::Node* parent = oldNode->GetParent();

	// We'll need the panel and tree.
	DockingPanel* dockingPanel = GetAssociatedPanel();
	Tree* tree = dockingPanel->GetTree();

	// If there's a panel up for the old node, deactivate it, because the old node is going away.
	dockingPanel->DeactivateNodeControlPanelForNode( oldNode.get(), true, true );

	// Pull off the sub-tree rooted at the old node.
	Cornucopia::Node::List::const_iterator insertBeforeIter;
	if( parent )
		oldNode->DetachFromParent( wxGetApp().GetContext(), &insertBeforeIter );
	else
		tree->SetRoot(0);

	// Transfer all the children of the old node to the new node.
	// The new node should receive them in the same order as the old node.
	Cornucopia::Node::List* nodeList = oldNode->GetChildren();
	if( nodeList )
	{
		while( nodeList->size() > 0 )
		{
			Cornucopia::Node::List::iterator iter = nodeList->begin();
			Cornucopia::Node* child = *iter;
			child->DetachFromParent( wxGetApp().GetContext() );
			child->AttachToParent( newNode.get(), wxGetApp().GetContext() );
		}
	}

	// Now attach the new node to the tree.
	if( parent )
		newNode->AttachToParent( parent, wxGetApp().GetContext(), &insertBeforeIter );
	else
		tree->SetRoot( newNode.get() );

	// Tell the tree control we added a new item.
	// Notice that we prevent the scope delete of the new node here too!
	// We also just let the scope delete happen for the old node.
	wxDataViewModel* dataViewModel = GetModel();
	wxDataViewItem parentItem( parent );
	wxDataViewItem oldItem( oldNode.get() );
	wxDataViewItem newItem( newNode.release() );
	dataViewModel->ItemDeleted( parentItem, oldItem );	// Maybe this one isn't necessary?
	dataViewModel->ItemAdded( parentItem, newItem );

	// Make sure we update the modification status of the panel.
	tree->SetModified( true );
	NoteBook* noteBook = wxGetApp().GetFrame()->GetNoteBook();
	noteBook->UpdateTabTitle( dockingPanel );

	// Add to the undo-redo system.
	dockingPanel->TakeSnapShot();
}

//=================================================================================
void DataViewCtrl::OnContextMenuCut( wxCommandEvent& event )
{
	// Do we have something to cut?
	wxDataViewItem selectedItem = GetSelection();
	if( !selectedItem.IsOk() )
		return;

	// Cut the node out of the tree.
	DockingPanel* dockingPanel = GetAssociatedPanel();
	Tree* tree = dockingPanel->GetTree();
	Cornucopia::Node* node = ( Cornucopia::Node* )selectedItem.m_pItem;
	Cornucopia::Node* parent = node->GetParent();
	if( !parent )
		tree->SetRoot(0);
	else
		node->DetachFromParent( wxGetApp().GetContext() );

	// Try to copy the node to the clip-board.
	if( !wxGetApp().CopyToClipBoard( node ) )
		wxMessageBox( wxT( "Failed to copy to clip-board." ), wxT( "Error" ), wxOK | wxCENTRE );

	// Be sure to bring down any panels for the cut sub-tree.
	dockingPanel->DeactivateNodeControlPanelsForSubTree( node, true, true );

	// Now that the sub-tree is safely on the clip-board, we can delete it.
	// The user can undo the action if the clip-board paste failed.
	delete node;

	// Add to the undo-redo system.
	dockingPanel->TakeSnapShot();

	// Tell the tree control what happened.
	wxDataViewModel* dataViewModel = GetModel();
	wxDataViewItem parentItem( parent );
	wxDataViewItem deletedItem( node );		// The stale pointer is okay.
	dataViewModel->ItemDeleted( parentItem, deletedItem );

	// Make sure we update the modification status of the panel.
	tree->SetModified( true );
	NoteBook* noteBook = wxGetApp().GetFrame()->GetNoteBook();
	noteBook->UpdateTabTitle( dockingPanel );
}

//=================================================================================
// There's no need to clone the node or take a snap-shot for the undo/redo system here.
void DataViewCtrl::OnContextMenuDeepCopy( wxCommandEvent& event )
{
	// Do we have something to copy?
	wxDataViewItem selectedItem = GetSelection();
	if( !selectedItem.IsOk() )
		return;

	// Try to copy the node to the clip-board.
	Cornucopia::Node* node = ( Cornucopia::Node* )selectedItem.m_pItem;
	if( !wxGetApp().CopyToClipBoard( node ) )
		wxMessageBox( wxT( "Failed to copy to clip-board." ), wxT( "Error" ), wxOK | wxCENTRE );
}

//=================================================================================
void DataViewCtrl::OnContextMenuShallowCopy( wxCommandEvent& event )
{
	// Do we have something to copy?
	wxDataViewItem selectedItem = GetSelection();
	if( !selectedItem.IsOk() )
		return;

	// Create a shallow copy of the node.
	Cornucopia::Node* node = ( Cornucopia::Node* )selectedItem.m_pItem;
	node = node->Clone( wxGetApp().GetContext(), Cornucopia::Node::COPY_NODE_AND_REMOVE_ALL_CHILDREN );

	// Try to copy the node to the clip-board.
	if( !wxGetApp().CopyToClipBoard( node ) )
		wxMessageBox( wxT( "Failed to copy to clip-board." ), wxT( "Error" ), wxOK | wxCENTRE );

	// Now that it's on the clip-board, we can delete it.
	delete node;
}

//=================================================================================
void DataViewCtrl::OnContextMenuPasteUnder( wxCommandEvent& event )
{
	wxDataViewItem selectedItem = GetSelection();

	// Try to create a sub-tree from the clip-board data.
	wxScopedPtr< Cornucopia::Node > node;
	node.reset( wxGetApp().CreateFromClipBoard() );

	// Bail if we didn't grab anything.
	if( !node.get() )
	{
		wxMessageBox( wxT( "Failed to copy from clip-board." ), wxT( "Error" ), wxOK | wxCENTRE );
		return;
	}
	
	// Try to attach the newly created node into the tree.
	bool success = false;
	DockingPanel* dockingPanel = GetAssociatedPanel();
	Tree* tree = dockingPanel->GetTree();
	Cornucopia::Node* parent = ( Cornucopia::Node* )selectedItem.m_pItem;
	if( parent )
		success = node.get()->AttachToParent( parent, wxGetApp().GetContext() );
	else
	{
		// We can't paste at the root if there is already a root.
		if( !tree->GetRoot() )
		{
			tree->SetRoot( node.get() );
			success = true;
		}
	}
	
	// Did we succeed?
	if( success )
	{
		// Yes.  Add to the undo-redo system.
		dockingPanel->TakeSnapShot();

		// Tell the tree control what happened.
		wxDataViewModel* dataViewModel = GetModel();
		wxDataViewItem parentItem( parent );
		wxDataViewItem newItem( node.get() );
		dataViewModel->ItemAdded( parentItem, newItem );

		// Make sure they can see the paste.
		Expand( parentItem );

		// Make sure we update the modification status of the panel.
		tree->SetModified( true );
		NoteBook* noteBook = wxGetApp().GetFrame()->GetNoteBook();
		noteBook->UpdateTabTitle( dockingPanel );

		// Lastly, keep the node in scope.
		node.release();
	}
}

//=================================================================================
// This operation is equivilant to deleting the sub-tree to be pasted
// over, then pasting the new sub-tree under the parent of the old tree.
void DataViewCtrl::OnContextMenuPasteOver( wxCommandEvent& event )
{
	wxDataViewItem selectedItem = GetSelection();
	if( !selectedItem.IsOk() )
		return;

	// Rip out the sub-tree.
	DockingPanel* dockingPanel = GetAssociatedPanel();
	Tree* tree = dockingPanel->GetTree();
	Cornucopia::Node* oldNode = ( Cornucopia::Node* )selectedItem.GetID();
	Cornucopia::Node* parent = oldNode->GetParent();
	Cornucopia::Node::List::const_iterator insertedBeforeIter;
	if( parent )
		oldNode->DetachFromParent( wxGetApp().GetContext(), &insertedBeforeIter );
	else
		tree->SetRoot(0);

	// Don't let panels persist that would have a stale node pointer.
	dockingPanel->DeactivateNodeControlPanelsForSubTree( oldNode, true, true );

	// Okay, go ahead and blow away the sub-tree.
	delete oldNode;
	
	// Update the data view control.
	wxDataViewModel* dataViewModel = GetModel();
	wxDataViewItem parentItem( parent );
	wxDataViewItem deletedItem( oldNode );		// wxWidgets doesn't de-ref the now-stale pointer, nor will it ask us to do so.
	dataViewModel->ItemDeleted( parentItem, deletedItem );

	// Okay, now try to create a sub-tree from the current clip-board data.
	wxScopedPtr< Cornucopia::Node > newNode;
	newNode.reset( wxGetApp().CreateFromClipBoard() );

	// Did we grab anything?
	if( newNode.get() )
	{
		// Yes.  Slap it into the same place where we uprooted the old sub-tree.
		bool success = false;
		if( parent )
			success = newNode->AttachToParent( parent, wxGetApp().GetContext(), &insertedBeforeIter );
		else
			success = tree->SetRoot( newNode.get() );

		// Make sure we don't leak memory on failure by checking here for success to release the scoped pointer.
		if( success )
		{
			// Tell the data-view control about the new structural change.
			wxDataViewItem newItem( newNode.get() );
			dataViewModel->ItemAdded( parentItem, newItem );
			Expand( parentItem );

			// Lastly, release the newly created node from deletion-doom at end-of-scope.
			newNode.release();
		}
	}

	// Whether we succeeded or now at this point, do the usual updates after a change has been made.
	tree->SetModified( true );
	NoteBook* noteBook = wxGetApp().GetFrame()->GetNoteBook();
	noteBook->UpdateTabTitle( dockingPanel );
	dockingPanel->TakeSnapShot();
}

//=================================================================================
void DataViewCtrl::OnContextMenuDelete( wxCommandEvent& event )
{
	// Do we have something to delete?
	wxDataViewItem selectedItem = GetSelection();
	if( !selectedItem.IsOk() )
		return;

	// Seperate the sub-tree from the rest of the tree.
	DockingPanel* dockingPanel = GetAssociatedPanel();
	Tree* tree = dockingPanel->GetTree();
	Cornucopia::Node* node = ( Cornucopia::Node* )selectedItem.GetID();
	Cornucopia::Node* parent = node->GetParent();
	if( !parent )
	{
		if( !tree->SetRoot(0) )
		{
			wxMessageBox( wxString::Format( wxT( "Failed to remove root of tree for file \"%s\"." ), tree->GetFilePath() ), wxT( "Error" ), wxOK | wxCENTRE );
			return;
		}
	}
	else
	{
		if( !node->DetachFromParent( wxGetApp().GetContext() ) )
		{
			wxGetApp().CornucopiaErrorMessageBox();
			return;
		}
	}

	// Destroy any floating control panels for nodes in the sub-tree.
	dockingPanel->DeactivateNodeControlPanelsForSubTree( node, true, true );

	// Blow the sub-tree away.
	delete node;

	// Tell the tree control what happened.
	wxDataViewModel* dataViewModel = GetModel();
	wxDataViewItem parentItem( parent );
	wxDataViewItem deletedItem( node );		// The stale pointer is okay, because it's just used as an ID by wxWidgets, and it won't give us the pointer for us to der-ref it either.
	dataViewModel->ItemDeleted( parentItem, deletedItem );

	// Make sure we update the modification status of the panel.
	tree->SetModified( true );
	NoteBook* noteBook = wxGetApp().GetFrame()->GetNoteBook();
	noteBook->UpdateTabTitle( dockingPanel );

	// Add to the undo-redo system.
	dockingPanel->TakeSnapShot();
}

//=================================================================================
void DataViewCtrl::OnContextMenuMoveForward( wxCommandEvent& event )
{
	MoveNodeAmongSiblings( true );
}

//=================================================================================
void DataViewCtrl::OnContextMenuMoveBackward( wxCommandEvent& event )
{
	MoveNodeAmongSiblings( false );
}

//=================================================================================
void DataViewCtrl::MoveNodeAmongSiblings( bool forward )
{
	wxDataViewItem selectedItem = GetSelection();
	if( !selectedItem.IsOk() )
		return;

	Cornucopia::Node* node = ( Cornucopia::Node* )selectedItem.m_pItem;
	Cornucopia::Node* parent = node->GetParent();
	if( !parent )
		return;

	Cornucopia::Node::List::const_iterator insertBeforeIter;
	if( !node->DetachFromParent( wxGetApp().GetContext(), &insertBeforeIter ) )
	{
		wxGetApp().CornucopiaErrorMessageBox();
		return;
	}

	wxDataViewModel* dataViewModel = GetModel();
	wxDataViewItem parentItem( parent );
	wxDataViewItem item( node );
	dataViewModel->ItemDeleted( parentItem, item );

	bool moveHappened = true;
	if( forward )
	{
		if( insertBeforeIter != parent->GetChildren()->end() )
			insertBeforeIter++;
		else
			moveHappened = false;
	}
	else
	{
		if( insertBeforeIter != parent->GetChildren()->begin() )
			insertBeforeIter--;
		else
			moveHappened = false;
	}

	if( node->AttachToParent( parent, wxGetApp().GetContext(), &insertBeforeIter ) )
		dataViewModel->ItemAdded( parentItem, item );
	else
	{
		delete node;
		wxGetApp().CornucopiaErrorMessageBox();
	}
	
	DockingPanel* dockingPanel = GetAssociatedPanel();
	Tree* tree = dockingPanel->GetTree();
	tree->SetModified( true );
	NoteBook* noteBook = wxGetApp().GetFrame()->GetNoteBook();
	noteBook->UpdateTabTitle( dockingPanel );

	if( moveHappened )
		dockingPanel->TakeSnapShot();
}

//=================================================================================
void DataViewCtrl::OnContextMenuToggleVisibility( wxCommandEvent& event )
{
	wxDataViewItem selectedItem = GetSelection();
	if( !selectedItem.IsOk() )
		return;

	Cornucopia::Node* node = ( Cornucopia::Node* )selectedItem.m_pItem;
	EditorMetaData* editorMetaData = ( EditorMetaData* )node->GetMetaData();
	editorMetaData->SetVisibility( !editorMetaData->GetVisibility() );

	DockingPanel* dockingPanel = GetAssociatedPanel();
	Tree* tree = dockingPanel->GetTree();
	tree->SetModified( true );
	NoteBook* noteBook = wxGetApp().GetFrame()->GetNoteBook();
	noteBook->UpdateTabTitle( dockingPanel );
}

//=================================================================================
void DataViewCtrl::OnContextMenuEditName( wxCommandEvent& event )
{
	wxDataViewItem selectedItem = GetSelection();
	if( selectedItem.IsOk() )
	{
		wxDataViewColumn* column = GetColumn( DataViewModel::COL_NAME );
		EditItem( selectedItem, column );
	}
}

//=================================================================================
void DataViewCtrl::OnContextMenuEditType( wxCommandEvent& event )
{
	wxDataViewItem selectedItem = GetSelection();
	if( selectedItem.IsOk() )
	{
		wxDataViewColumn* column = GetColumn( DataViewModel::COL_TYPE );
		EditItem( selectedItem, column );
	}
}

//=================================================================================
void DataViewCtrl::OnContextMenuEditValue( wxCommandEvent& event )
{
	wxDataViewItem selectedItem = GetSelection();
	if( selectedItem.IsOk() )
	{
		wxDataViewColumn* column = GetColumn( DataViewModel::COL_VALUE );
		EditItem( selectedItem, column );
	}
}

//=================================================================================
void DataViewCtrl::OnContextMenuEditShortExplanation( wxCommandEvent& event )
{
	wxDataViewItem selectedItem = GetSelection();
	if( selectedItem.IsOk() )
	{
		Cornucopia::Node* node = ( Cornucopia::Node* )selectedItem.m_pItem;
		wxString shortExplanation = ( const char* )node->GetExplanation( Cornucopia::Node::SHORT_EXPLANATION ).c_str();
		wxTextEntryDialog textEntryDialog( wxGetApp().GetFrame(), wxT( "Enter short explanation." ), wxT( "Short Explanation" ), shortExplanation );
		if( textEntryDialog.ShowModal() == wxID_OK )
		{
			shortExplanation = textEntryDialog.GetValue();
			node->SetExplanation( Cornucopia::Node::SHORT_EXPLANATION, ( const char* )shortExplanation.c_str() );
		}
	}
}

//=================================================================================
void DataViewCtrl::OnContextMenuEditLongExplanation( wxCommandEvent& event )
{
	wxDataViewItem selectedItem = GetSelection();
	if( selectedItem.IsOk() )
	{
		Cornucopia::Node* node = ( Cornucopia::Node* )selectedItem.m_pItem;
		wxString longExplanation = ( const char* )node->GetExplanation( Cornucopia::Node::LONG_EXPLANATION ).c_str();
		wxTextEntryDialog textEntryDialog( wxGetApp().GetFrame(), wxT( "Enter long explanation." ), wxT( "Long Explanation" ), longExplanation );
		if( textEntryDialog.ShowModal() == wxID_OK )
		{
			longExplanation = textEntryDialog.GetValue();
			node->SetExplanation( Cornucopia::Node::LONG_EXPLANATION, ( const char* )longExplanation.c_str() );
		}
	}
}

//=================================================================================
void DataViewCtrl::OnContextMenuToggleColumn( wxCommandEvent& event )
{
	int modelCol = -1;
	switch( event.GetId() )
	{
		case ID_ContextMenu_ToggleNameColumn:		modelCol = DataViewModel::COL_NAME;		break;
		case ID_ContextMenu_ToggleTypeColumn:		modelCol = DataViewModel::COL_TYPE;		break;
		case ID_ContextMenu_ToggleValueColumn:		modelCol = DataViewModel::COL_VALUE;	break;
		case ID_ContextMenu_ToggleFeedbackColumn:	modelCol = DataViewModel::COL_FEEDBACK;	break;
	}
	if( modelCol == -1 )
		return;

	shownColumnFlags ^= 1 << modelCol;

	RebuildColumns();
}

//=================================================================================
// Hmmmm.  This produces an undo/redo snap-shot, even if the value of the
// node was already at its default.
void DataViewCtrl::OnContextMenuRestoreDefaultValue( wxCommandEvent& event )
{
	wxDataViewItem selectedItem = GetSelection();
	if( selectedItem.IsOk() )
	{
		// Attempt to perform the default value restoration.
		Cornucopia::Node* node = ( Cornucopia::Node* )selectedItem.GetID();
		if( !node->RestoreDefaultValue( wxGetApp().GetContext() ) )
		{
			wxGetApp().CornucopiaErrorMessageBox( wxString::Format( "Failed to restore default value for node \"%s\".", node->GetName().c_str() ) );
			return;
		}

		// Update the UI accordingly.
		NodeValueChangedByThisControl( node, true, false );
	}
}

//=================================================================================
// Editing is about to start on a column.  Here we have the chance
// to stop that from happening, or let it happen.
void DataViewCtrl::OnStartEditing( wxDataViewEvent& event )
{
	wxDataViewItem item = event.GetItem();
	Cornucopia::Node* node = ( Cornucopia::Node* )item.m_pItem;
	EditorMetaData* editorMetaData = ( EditorMetaData* )node->GetMetaData();
	bool updateItem = false;

	wxDataViewColumn* dataViewColumn = event.GetDataViewColumn();
	if( dataViewColumn->GetTitle() == wxT( "Name" ) )
	{
		// Just let it happen.
	}
	else if( dataViewColumn->GetTitle() == wxT( "Type" ) )
	{
		editorMetaData->SetFeedback( "You cannot edit node type here.  Use the context menu to alter a given node's type." );
		event.Veto();
		updateItem = true;
	}
	else if( dataViewColumn->GetTitle() == wxT( "Value" ) )
	{
		// Is the setting and getting of the value of the node by string supported?
		std::string helpString;
		if( Cornucopia::Node::VSE_NOT_SUPPORTED == node->GetValueAsStringHelp( helpString ) )
		{
			// No.  Tell the user and veto the edit.
			editorMetaData->SetFeedback( "Edit by string not supported by this node type." );
			event.Veto();
		}
		else
		{
			// Yes.  Show a string value that they can then edit, if they want, which is already parsable by the node.
			editorMetaData->SetValueDisplayMode( EditorMetaData::SHOW_VALUE_AS_STRING );
		}

		updateItem = true;
	}
	else if( dataViewColumn->GetTitle() == wxT( "Feedback" ) )
	{
		editorMetaData->SetFeedback( "The feedback column is for user feedback only.  You can't edit it." );
		event.Veto();
		updateItem = true;
	}

	if( updateItem )
	{
		wxDataViewModel* dataViewModel = GetModel();
		dataViewModel->ItemChanged( item );
	}
}

//=================================================================================
void DataViewCtrl::OnEditingStarted( wxDataViewEvent& event )
{
	wxDataViewColumn* dataViewColumn = event.GetDataViewColumn();
	if( dataViewColumn->GetTitle() == wxT( "Value" ) )
	{
		wxDataViewItem item = event.GetItem();
		Cornucopia::Node* node = ( Cornucopia::Node* )item.m_pItem;
		EditorMetaData* editorMetaData = ( EditorMetaData* )node->GetMetaData();
				
		// Display some help on setting the value by string.
		std::string helpString;
		node->GetValueAsStringHelp( helpString );
		editorMetaData->SetFeedback( ( const char* )helpString.c_str() );

		wxDataViewModel* dataViewModel = GetModel();
		dataViewModel->ItemChanged( item );
	}
}

//=================================================================================
// NOTICE: I've discovered a bug with cell editing the data-view control that I
//         believe is a wxWidgets bug.  One way to get it to happen is to start
//         editing a cell, resize the column header to expose some space behind
//         the text edit box, then quickly left click on that space and hit F2.
//         The edit box will then remain up with no way of closing it down.
void DataViewCtrl::OnEditingDone( wxDataViewEvent& event )
{
	wxDataViewItem item = event.GetItem();
	Cornucopia::Node* node = ( Cornucopia::Node* )item.m_pItem;
	EditorMetaData* editorMetaData = ( EditorMetaData* )node->GetMetaData();

	wxString userValue = event.GetValue().GetString();
	std::string valueString = ( const char* )userValue.c_str();
	Cornucopia::Node::ValueStringError valueStringError = Cornucopia::Node::VSE_NONE;

	int column = event.GetColumn();
	switch( column )
	{
		case DataViewModel::COL_VALUE:
		{
			// Try to set the node by the given string.
			valueStringError = node->SetValueFromString( valueString );
			break;
		}
		case DataViewModel::COL_NAME:
		{
			if( node->GetName() == valueString )
				valueStringError = Cornucopia::Node::VSE_NO_CHANGE;
			else
			{
				node->SetName( valueString );
				valueStringError = Cornucopia::Node::VSE_NONE;
			}
			break;
		}
	}

	if( valueStringError == Cornucopia::Node::VSE_NO_CHANGE )
		editorMetaData->SetFeedback( "Value not changed." );
	else if( valueStringError == Cornucopia::Node::VSE_INVALID )
		editorMetaData->SetFeedback( "Value rejected!  (Illegal value.)" );
	else if( valueStringError == Cornucopia::Node::VSE_SYNTAX )
		editorMetaData->SetFeedback( "Value rejected!  (Syntax error.)" );
	else if( valueStringError == Cornucopia::Node::VSE_NONE )
	{
		editorMetaData->SetFeedback( "Value accepted." );
		NodeValueChangedByThisControl( node, false, ( column == DataViewModel::COL_NAME ? true : false ) );
	}

	editorMetaData->SetValueDisplayMode( EditorMetaData::SHOW_DISPLAY_VALUE );

	wxDataViewModel* dataViewModel = GetModel();
	dataViewModel->ItemChanged( item );
}

//=================================================================================
// If this control ever changes the value of a node, this routine should
// be called to update the entire UI of the owning panel.
void DataViewCtrl::NodeValueChangedByThisControl( Cornucopia::Node* node, bool updateDataViewItem, bool updatePanelCaption )
{
	if( updateDataViewItem )
	{
		wxDataViewModel* dataViewModel = GetModel();
		wxDataViewItem changedItem( node );
		dataViewModel->ItemChanged( changedItem );
	}

	DockingPanel* dockingPanel = GetAssociatedPanel();

	EditorMetaData* editorMetaData = ( EditorMetaData* )node->GetMetaData();
	NodeControlPanel* nodeControlPanel = editorMetaData->GetNodeControlPanel();
	if( nodeControlPanel )
	{
		nodeControlPanel->PushNodeDataToInterface();

		if( updatePanelCaption )
		{
			wxAuiManager* auiManager = dockingPanel->GetAuiManager();
			auiManager->GetPane( nodeControlPanel ).Caption( ( const char* )node->GetName().c_str() );
			auiManager->Update();
		}
	}
	
	Tree* tree = dockingPanel->GetTree();
	tree->SetModified( true );
	NoteBook* noteBook = wxGetApp().GetFrame()->GetNoteBook();
	noteBook->UpdateTabTitle( dockingPanel );

	// Don't forget to add to the undo/redo system.
	dockingPanel->TakeSnapShot();
}

// DataViewCtrl.cpp