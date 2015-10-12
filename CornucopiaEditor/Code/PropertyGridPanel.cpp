// PropertyGridPanel.cpp

#include "CornucopiaEditor.h"

using namespace CornucopiaEditor;

wxIMPLEMENT_DYNAMIC_CLASS( PropertyGridPanel, Panel )

// TODO: We should be saveing and restoring expansion state in the node meta-data.

//=================================================================================
PropertyGridPanel::PropertyGridPanel( void )
{
	propertyGrid = 0;
}

//=================================================================================
PropertyGridPanel::PropertyGridPanel( wxWindow* parent, Tree* tree ) : Panel( parent, tree )
{
	propertyGrid = 0;
}

//=================================================================================
/*virtual*/ PropertyGridPanel::~PropertyGridPanel( void )
{
}

//=================================================================================
/*virtual*/ void PropertyGridPanel::BuildUserInterface( void )
{
	propertyGrid = new wxPropertyGrid( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxPG_SPLITTER_AUTO_CENTER );
	propertyGrid->SetExtraStyle( wxPG_EX_HELP_AS_TOOLTIPS );

	propertyGrid->Bind( wxEVT_PG_CHANGED, &PropertyGridPanel::OnPropertyValueChanged, this );
	propertyGrid->Bind( wxEVT_PG_DOUBLE_CLICK, &PropertyGridPanel::OnPropertyGridDoubleClick, this );

	RebuildPropertyGrid();
	RefreshPropertyGrid();

	wxBoxSizer* boxSizer = new wxBoxSizer( wxVERTICAL );
	boxSizer->Add( propertyGrid, 1, wxGROW | wxALL, 0 );
	SetSizer( boxSizer );
}

//=================================================================================
/*virtual*/ void PropertyGridPanel::RefreshUserInterface( void )
{
	Freeze();
	RefreshPropertyGrid();
	RefreshAllDialogs();
	Thaw();
}

//=================================================================================
/*virtual*/ void PropertyGridPanel::PreUndoRedoNotice( Cornucopia::Node* newTree, Cornucopia::Node* forgottenTree )
{
}

//=================================================================================
/*virtual*/ void PropertyGridPanel::PostUndoRedoNotice( Cornucopia::Node* oldTree, Cornucopia::Node* forgottenTree )
{
	// Though our interface does not support structural changes to the tree, the
	// structure of the tree can actually change on undo/redo, because our history
	// can be created by a different interface style which does support tree structure
	// changes, so we need to account for the possibility here that we can't create a bridge.
	Cornucopia::Node* newTree = tree->GetRoot();
	if( CreateBridge( oldTree, newTree ) )
	{
		// Go connect up the new tree to the property grid.  This is faster than rebuild the property grid altogether.
		for( wxPropertyGridIterator iter = propertyGrid->GetIterator(); !iter.AtEnd(); iter++ )
		{
			wxPGProperty* property = *iter;
			NodeProperty* nodeProperty = NodeProperty::LameCast( property );
			Cornucopia::Node* oldNode = nodeProperty->GetNode();
			EditorMetaData* editorMetaData = ( EditorMetaData* )oldNode->GetMetaData();
			Cornucopia::Node* newNode = editorMetaData->GetBridgeNode();
			editorMetaData = ( EditorMetaData* )newNode->GetMetaData();
			editorMetaData->SetProperty( property );
			nodeProperty->SetNode( newNode );
		}

		// Go connect all the dialogs to the new tree too.
		for( DialogList::iterator iter = dialogList.begin(); iter != dialogList.end(); iter++ )
		{
			NodeControlPanel::Dialog* dialog = *iter;
			NodeControlPanel* nodeControlPanel = dialog->GetNodeControlPanel();
			Cornucopia::Node* oldNode = nodeControlPanel->GetNode();
			EditorMetaData* editorMetaData = ( EditorMetaData* )oldNode->GetMetaData();
			Cornucopia::Node* newNode = editorMetaData->GetBridgeNode();
			nodeControlPanel->SetNode( newNode );
		}
	}
	else
	{
		// If we couldn't create a bridge, then here we need to simply rebuild the whole property grid.
		RebuildPropertyGrid();

		// We have no way of associating currently open dialogs with new nodes, so shut'em down.
		DestroyAllDialogs();
	}

	// To be clean, always zero out property pointers in the old tree so that we don't keep potentially stale pointers around.
	Cornucopia::Node::List nodeList;
	if( oldTree )
	{
		oldTree->MakeNodeList( nodeList );
		for( Cornucopia::Node::List::iterator iter = nodeList.begin(); iter != nodeList.end(); iter++ )
		{
			Cornucopia::Node* node = *iter;
			EditorMetaData* editorMetaData = ( EditorMetaData* )node->GetMetaData();
			editorMetaData->SetProperty(0);
		}
	}

	RefreshUserInterface();

	Tree* tree = GetTree();
	tree->SetModified( true );
	NoteBook* noteBook = wxGetApp().GetFrame()->GetNoteBook();
	noteBook->UpdateTabTitle( this );
}

//=================================================================================
void PropertyGridPanel::RebuildPropertyGrid( void )
{
	propertyGrid->Clear();

	Cornucopia::Node* root = tree->GetRoot();
	if( root )
		BuildPropertyGridRecursively( 0, root );
}

//=================================================================================
void PropertyGridPanel::BuildPropertyGridRecursively( wxPGProperty* parentProperty, Cornucopia::Node* node )
{
	wxPGProperty* property = CreatePropertyForNode( node );
	if( !property )
		return;

	if( parentProperty )
		propertyGrid->AppendIn( parentProperty, property );
	else
		propertyGrid->Append( property );

	Cornucopia::Node::List* nodeList = node->GetChildren();
	if( nodeList )
	{
		for( Cornucopia::Node::List::iterator iter = nodeList->begin(); iter != nodeList->end(); iter++ )
		{
			Cornucopia::Node* child = *iter;
			BuildPropertyGridRecursively( property, child );
		}
	}
}

//=================================================================================
void PropertyGridPanel::RefreshPropertyGrid( void )
{
	for( wxPropertyGridIterator iter = propertyGrid->GetIterator(); !iter.AtEnd(); iter++ )
	{
		wxPGProperty* property = *iter;
		NodeProperty* nodeProperty = NodeProperty::LameCast( property );
		if( nodeProperty )
			nodeProperty->PushNodeValueToProperty();
	
		PluginRefreshProperty( property );
	}
}

//=================================================================================
void PropertyGridPanel::PluginRefreshProperty( wxPGProperty* property )
{
	PluginManager* pluginManager = wxGetApp().GetPluginManager();
	if( pluginManager )
	{
		PluginPropertyEvent event;
		event.SetEventType( CORNUCOPIA_PLUGIN_PROPERTY_REFRESH );
		event.SetEventObject( this );
		event.SetProperty( property );
		event.SetKeyColumn(0);
		event.SetValueColumn(1);
		event.SetNode( GetPropertyToNode( property ) );
		if( pluginManager->CallAllPlugins( event ) )
			propertyGrid->RefreshProperty( property );
	}
}

//=================================================================================
void PropertyGridPanel::RefreshAllDialogs( void )
{
	for( DialogList::iterator iter = dialogList.begin(); iter != dialogList.end(); iter++ )
	{
		NodeControlPanel::Dialog* dialog = *iter;
		NodeControlPanel* nodeControlPanel = dialog->GetNodeControlPanel();
		nodeControlPanel->PushNodeDataToInterface();
	}
}

//=================================================================================
void PropertyGridPanel::OnPropertyValueChanged( wxPropertyGridEvent& event )
{
	wxPGProperty* property = event.GetProperty();
	NodeProperty* nodeProperty = NodeProperty::LameCast( property );
	if( nodeProperty )
	{
		if( !nodeProperty->PullNodeValueFromProperty() )
			nodeProperty->PushNodeValueToProperty();
		else
		{
			Tree* tree = GetTree();
			tree->SetModified( true );
			NoteBook* noteBook = wxGetApp().GetFrame()->GetNoteBook();
			noteBook->UpdateTabTitle( this );

			Cornucopia::Node* node = nodeProperty->GetNode();
			EditorMetaData* editorMetaData = ( EditorMetaData* )node->GetMetaData();
			NodeControlPanel* nodeControlPanel = editorMetaData->GetNodeControlPanel();
			if( nodeControlPanel )
				nodeControlPanel->PushNodeDataToInterface();

			PluginRefreshProperty( property );
			SendLiveTweakEvent( node, false );

			TakeSnapShot();
		}
	}
}

//=================================================================================
void PropertyGridPanel::OnPropertyGridDoubleClick( wxPropertyGridEvent& event )
{
	wxPGProperty* property = event.GetProperty();
	NodeProperty* nodeProperty = NodeProperty::LameCast( property );
	if( nodeProperty )
	{
		Cornucopia::Node* node = nodeProperty->GetNode();
		ShowDialogForNode( node );
	}
}

//=================================================================================
void PropertyGridPanel::ShowDialogForNode( Cornucopia::Node* node )
{
	NodeControlPanel::Dialog* dialog = FindDialogForNode( node );
	if( !dialog )
	{
		wxString title = node->GetName();

		// wxWidgets will take ownership of the dialog memory, so we don't have to delete these dialogs.
		dialog = new NodeControlPanel::Dialog( node, this, wxID_ANY, title, wxDefaultPosition, wxDefaultSize );
		
		// We don't need to worry about removing dialogs from the list.  Think of it like a cache.
		dialogList.push_back( dialog );
	}

	dialog->Show();
}

//=================================================================================
NodeControlPanel::Dialog* PropertyGridPanel::FindDialogForNode( Cornucopia::Node* node )
{
	NodeControlPanel::Dialog* foundDialog = 0;
	for( DialogList::iterator iter = dialogList.begin(); iter != dialogList.end() && !foundDialog; iter++ )
	{
		NodeControlPanel::Dialog* dialog = *iter;
		NodeControlPanel* nodeControlPanel = dialog->GetNodeControlPanel();
		if( nodeControlPanel && nodeControlPanel->GetNode() == node )
			foundDialog = dialog;
	}
	return foundDialog;
}

//=================================================================================
void PropertyGridPanel::DestroyAllDialogs( void )
{
	while( dialogList.size() > 0 )
	{
		DialogList::iterator iter = dialogList.begin();
		NodeControlPanel::Dialog* dialog = *iter;
		dialog->Destroy();
		dialogList.erase( iter );
	}
}

//=================================================================================
/*virtual*/ void PropertyGridPanel::SynchronizeControlsWithNode( Cornucopia::Node* node )
{
	EditorMetaData* editorMetaData = ( EditorMetaData* )node->GetMetaData();
	wxPGProperty* property = editorMetaData->GetProperty();
	NodeProperty* nodeProperty = NodeProperty::LameCast( property );
	nodeProperty->PushNodeValueToProperty();
	propertyGrid->RefreshProperty( property );
}

//=================================================================================
wxPGProperty* PropertyGridPanel::CreatePropertyForNode( Cornucopia::Node* node )
{
	return NodeProperty::Create( node );
}

//=================================================================================
wxPGProperty* PropertyGridPanel::GetPropertyFromNode( Cornucopia::Node* node )
{
	EditorMetaData* editorMetaData = ( EditorMetaData* )node->GetMetaData();
	wxPGProperty* property = editorMetaData->GetProperty();
	return property;
}

//=================================================================================
Cornucopia::Node* PropertyGridPanel::GetPropertyToNode( wxPGProperty* property )
{
	Cornucopia::Node* node = 0;
	NodeProperty* nodeProperty = NodeProperty::LameCast( property );
	if( nodeProperty )
		node = nodeProperty->GetNode();
	return node;
}

//=================================================================================
/*virtual*/ bool PropertyGridPanel::FindNext( const wxString& searchCriteria )
{
	wxPGProperty* property = propertyGrid->GetSelection();
	bool isWild = wxIsWild( searchCriteria );

	wxPropertyGridIterator iter = propertyGrid->GetIterator( wxPG_ITERATE_DEFAULT, property );
	if( property )
		iter.Next();
	while( !iter.AtEnd() )
	{
		property = iter.GetProperty();
		wxString label = property->GetLabel();
		if( ( isWild && wxMatchWild( searchCriteria, label, false ) ) || label.CmpNoCase( searchCriteria ) == 0 )
			break;
		iter.Next();
	}

	if( iter.AtEnd() )
		return false;

	propertyGrid->ClearSelection();
	propertyGrid->SelectProperty( property );
	return true;
}

// PropertyGridPanel.cpp