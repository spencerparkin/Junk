// EditorMetaData.cpp

#include "CornucopiaEditor.h"

using namespace CornucopiaEditor;

// TODO: It might be handy to let node control panels save and restore data through the editor meta data.

//=================================================================================
EditorMetaData::EditorMetaData( void )
{
	expanded = false;
	visible = true;
	paneInfoString = wxT( "" );

	nodeControlPanelHandle = HandleSystem< NodeControlPanel >::INVALID_HANDLE;
	shouldActivateNodeControlPanel = false;

	feedback = wxT( "" );
	valueDisplayMode = SHOW_DISPLAY_VALUE;

	bridgeNode = 0;
	property = 0;
}

//=================================================================================
/*virtual*/ EditorMetaData::~EditorMetaData( void )
{
}

//=================================================================================
void EditorMetaData::SetBridgeNode( Cornucopia::Node* node )
{
	this->bridgeNode = node;
}

//=================================================================================
Cornucopia::Node* EditorMetaData::GetBridgeNode( void ) const
{
	return bridgeNode;
}

//=================================================================================
void EditorMetaData::SetProperty( wxPGProperty* property )
{
	this->property = property;
}

//=================================================================================
wxPGProperty* EditorMetaData::GetProperty( void )
{
	return property;
}

//=================================================================================
/*virtual*/ void EditorMetaData::PostCreate( Cornucopia::Node* node )
{
}

//=================================================================================
/*virtual*/ void EditorMetaData::PreDestroy( Cornucopia::Node* node )
{
	// Don't let node control panels hang on to stale node pointers.
	NodeControlPanel* nodeControlPanel = GetNodeControlPanel();
	if( nodeControlPanel && nodeControlPanel->GetNode() == node )
		nodeControlPanel->SetNode(0);
}

//=================================================================================
/*static*/ Cornucopia::MetaData* EditorMetaData::Create( void )
{
	return new EditorMetaData();
}

//=================================================================================
bool EditorMetaData::GetExpanded( void )
{
	return expanded;
}

//=================================================================================
void EditorMetaData::SetExpanded( bool expanded )
{
	this->expanded = expanded;
}

//=================================================================================
NodeControlPanel* EditorMetaData::GetNodeControlPanel( void ) const
{
	NodeControlPanel* nodeControlPanel = NodeControlPanel::DereferenceHandle( nodeControlPanelHandle );
	return nodeControlPanel;
}

//=================================================================================
void EditorMetaData::SetNodeControlPanel( NodeControlPanel* nodeControlPanel )
{
	if( nodeControlPanel )
		nodeControlPanelHandle = nodeControlPanel->GetHandle();
	else
		nodeControlPanelHandle = HandleSystem< NodeControlPanel >::INVALID_HANDLE;
}

//=================================================================================
int EditorMetaData::GetNodeControlPanelHandle( void ) const
{
	return nodeControlPanelHandle;
}

//=================================================================================
void EditorMetaData::SetNodeControlPanelHandle( int nodeControlPanelHandle )
{
	this->nodeControlPanelHandle = nodeControlPanelHandle;
}

//=================================================================================
wxString EditorMetaData::GetPaneInfoString( void )
{
	return paneInfoString;
}

//=================================================================================
void EditorMetaData::SetPaneInfoString( const wxString& paneInfoString )
{
	this->paneInfoString = paneInfoString;
}

//=================================================================================
/*virtual*/ bool EditorMetaData::ReadFromTable( lua_State* L, Cornucopia::Context& context )
{
	// Restore the expansion state of the node in the tree view control.
	lua_getfield( L, -1, "expanded" );
	if( lua_isnil( L, -1 ) || !lua_isboolean( L, -1 ) )
		expanded = false;
	else
		expanded = lua_toboolean( L, -1 ) ? true : false;
	lua_pop( L, 1 );

	// Restore the visibility state of the node.  Some tree interfaces honor this.
	lua_getfield( L, -1, "visible" );
	if( lua_isnil( L, -1 ) || !lua_isboolean( L, -1 ) )
		visible = true;
	else
		visible = lua_toboolean( L, -1 ) ? true : false;
	lua_pop( L, 1 );

	// Restore the pane layout info in the form of a string, if any.
	lua_getfield( L, -1, "paneInfo" );
	if( lua_isnil( L, -1 ) || !lua_isstring( L, -1 ) )
		paneInfoString = wxEmptyString;
	else
		paneInfoString = lua_tostring( L, -1 );
	lua_pop( L, 1 );

	// Restore the activation state of the pane.
	lua_getfield( L, -1, "shouldActivateNodeControlPanel" );
	if( lua_isnil( L, -1 ) || !lua_isboolean( L, -1 ) )
		shouldActivateNodeControlPanel = false;
	else
		shouldActivateNodeControlPanel = lua_toboolean( L, -1 ) ? true : false;
	lua_pop( L, 1 );

	// Return success.
	return true;
}

//=================================================================================
/*virtual*/ bool EditorMetaData::WriteToTable( lua_State* L, Cornucopia::Context& context ) const
{
	// Save the expansion state of the node in the tree view control.
	lua_pushboolean( L, expanded );
	lua_setfield( L, -2, "expanded" );

	// Save the visibility state.
	lua_pushboolean( L, visible );
	lua_setfield( L, -2, "visible" );

	// Save the pane layout for this node, if there is one.
	if( paneInfoString.IsEmpty() )
		lua_pushnil( L );
	else
		lua_pushstring( L, paneInfoString );
	lua_setfield( L, -2, "paneInfo" );

	// Remember the activation state of the pane.
	lua_pushboolean( L, shouldActivateNodeControlPanel );
	lua_setfield( L, -2, "shouldActivateNodeControlPanel" );

	// Return success.
	return true;
}

//=================================================================================
Panel* EditorMetaData::FindAssociatedPanel( void )
{
	Panel* panel = 0;
	NodeControlPanel* nodeControlPanel = GetNodeControlPanel();
	if( nodeControlPanel )
	{
		wxWindow* parent = nodeControlPanel->GetParent();
		while( parent && !panel )
		{
			panel = wxDynamicCast( parent, Panel );
			parent = parent->GetParent();
		}
	}
	return panel;
}

//=================================================================================
/*virtual*/ bool EditorMetaData::Copy( const MetaData* metaData, Cornucopia::Context& context )
{
	const EditorMetaData* editorMetaData = ( const EditorMetaData* )metaData;

	// Copy the expansion state and visibility state.
	expanded = editorMetaData->expanded;
	visible = editorMetaData->visible;

	// Notice that we do not attempt to patch the node control panel's node pointer here.
	// That will have to be the responsibility of the person performing the copy.
	nodeControlPanelHandle = editorMetaData->nodeControlPanelHandle;

	// Copy the pane info of the given node's node control panel, and the activation state.
	paneInfoString = editorMetaData->paneInfoString;
	shouldActivateNodeControlPanel = editorMetaData->shouldActivateNodeControlPanel;
	
	// Just set this value, since sometimes it doesn't always get reset properly.  Problem with wxWidgets control?
	valueDisplayMode = EditorMetaData::SHOW_DISPLAY_VALUE;

	return true;
}

//=================================================================================
wxString EditorMetaData::GetFeedback( void ) const
{
	return feedback;
}

//=================================================================================
void EditorMetaData::SetFeedback( const wxString& feedback )
{
	this->feedback = feedback;
}

//=================================================================================
EditorMetaData::ValueDisplayMode EditorMetaData::GetValueDisplayMode( void ) const
{
	return valueDisplayMode;
}

//=================================================================================
void EditorMetaData::SetValueDisplayMode( ValueDisplayMode valueDisplayMode )
{
	this->valueDisplayMode = valueDisplayMode;
}

//=================================================================================
void EditorMetaData::SetVisibility( bool visible )
{
	this->visible = visible;
}

//=================================================================================
bool EditorMetaData::GetVisibility( void ) const
{
	return visible;
}

//=================================================================================
bool EditorMetaData::GetShouldActivateNodeControlPanel( void )
{
	return shouldActivateNodeControlPanel;
}

//=================================================================================
void EditorMetaData::SetShouldActivateNodeControlPanel( bool shouldActivateNodeControlPanel )
{
	this->shouldActivateNodeControlPanel = shouldActivateNodeControlPanel;
}

// EditorMetaData.cpp