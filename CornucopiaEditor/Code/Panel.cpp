// Panel.cpp

#include "CornucopiaEditor.h"

using namespace CornucopiaEditor;

wxIMPLEMENT_DYNAMIC_CLASS( Panel, wxPanel )

//=================================================================================
// This default constructor should never be used.
Panel::Panel( void )
{
	tree = 0;
	undoRedo = 0;
}

//=================================================================================
Panel::Panel( wxWindow* parent, Tree* tree,
					const wxPoint& pos /*= wxDefaultPosition*/,
					const wxSize& size /*= wxDefaultSize*/ ) : wxPanel( parent, wxID_ANY, pos, size )
{
	this->tree = tree;
	undoRedo = 0;
}

//=================================================================================
/*virtual*/ Panel::~Panel( void )
{
	if( tree )
		delete tree;

	SetUndoRedoSystem(0);
}

//=================================================================================
/*virtual*/ void Panel::BuildUserInterface( void )
{
}

//=================================================================================
/*virtual*/ void Panel::RefreshUserInterface( void )
{
	RefreshNodeControlPanels();
}

//=================================================================================
Tree* Panel::GetTree( bool remove /*= false*/ )
{
	Tree* tree = this->tree;
	if( remove )
		this->tree = 0;
	return tree;
}

//=================================================================================
void Panel::RefreshNodeControlPanels( void )
{
	if( !tree )
		return;

	Cornucopia::Node* node = tree->GetRoot();
	if( !node )
		return;

	Cornucopia::Node::List nodeList;
	node->MakeNodeList( nodeList );

	for( Cornucopia::Node::List::iterator iter = nodeList.begin(); iter != nodeList.end(); iter++ )
	{
		node = *iter;

		EditorMetaData* editorMetaData = ( EditorMetaData* )node->GetMetaData();
		NodeControlPanel* nodeControlPanel = editorMetaData->GetNodeControlPanel();
		if( nodeControlPanel )
			nodeControlPanel->PushNodeDataToInterface();
	}
}

//=================================================================================
void Panel::PatchNodeControlPanels( void )
{
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
				NodeControlPanel* nodeControlPanel = editorMetaData->GetNodeControlPanel();
				if( nodeControlPanel )
				{
					// If the node control panel was always created from the node, or a copy of it, this node should be of the correct type.
					nodeControlPanel->SetNode( node );
				}
			}
		}
	}
}

//=================================================================================
/*static*/ void Panel::PatchNodeControlPanelsUsingBridge( Cornucopia::Node* root )
{
	if( root )
	{
		Cornucopia::Node::List nodeList;
		root->MakeNodeList( nodeList );
		for( Cornucopia::Node::List::iterator iter = nodeList.begin(); iter != nodeList.end(); iter++ )
		{
			Cornucopia::Node* node = *iter;
			EditorMetaData* editorMetaData = ( EditorMetaData* )node->GetMetaData();
			int nodeControlPanelHandle = editorMetaData->GetNodeControlPanelHandle();
			node = editorMetaData->GetBridgeNode();
			editorMetaData = ( EditorMetaData* )node->GetMetaData();
			editorMetaData->SetNodeControlPanelHandle( nodeControlPanelHandle );
			NodeControlPanel* nodeControlPanel = editorMetaData->GetNodeControlPanel();
			if( nodeControlPanel )
				nodeControlPanel->SetNode( node );
		}
	}
}

//=================================================================================
/*static*/ bool Panel::CreateBridge( Cornucopia::Node* node, Cornucopia::Node* targetNode )
{
	// Handle the vacuous case.
	if( !node && !targetNode )
		return true;

	// Handle a trivial case.
	if( !node || !targetNode )
		return false;

	// The given nodes must be of the same type.
	if( node->GetType() != targetNode->GetType() )
		return false;

	// Their children must be of the same type and in the same order.
	Cornucopia::Node::List* nodeList = node->GetChildren();
	Cornucopia::Node::List* targetNodeList = targetNode->GetChildren();
	if( nodeList && nodeList->size() > 0 )
	{
		if( !targetNodeList || targetNodeList->size() != nodeList->size() )
			return false;

		Cornucopia::Node::List::iterator iter = nodeList->begin();
		Cornucopia::Node::List::iterator targetIter = targetNodeList->begin();
		do
		{
			Cornucopia::Node* child = *iter;
			Cornucopia::Node* targetChild = *targetIter;

			if( !CreateBridge( child, targetChild ) )
				return false;

			iter++;
			targetIter++;
		}
		while( iter != nodeList->end() );
	}
	else if( targetNodeList && targetNodeList->size() > 0 )
		return false;

	EditorMetaData* editorMetaData = ( EditorMetaData* )node->GetMetaData();
	editorMetaData->SetBridgeNode( targetNode );
	return true;
}

//=================================================================================
/*virtual*/ void Panel::SynchronizeControlsWithNode( Cornucopia::Node* node )
{
}

//=================================================================================
void Panel::SendLiveTweakEvent( Cornucopia::Node* node, bool scrubbing )
{
	PluginManager* pluginManager = wxGetApp().GetPluginManager();
	if( pluginManager )
	{
		PluginTweakEvent event;
		if( scrubbing )
			event.SetEventType( CORNUCOPIA_PLUGIN_TWEAK_NODE_SCRUBBING );
		else
			event.SetEventType( CORNUCOPIA_PLUGIN_TWEAK_NODE_CHANGED );
		event.SetEventObject( this );
		event.SetNode( node );
		pluginManager->CallAllPlugins( event );
	}
}

//=================================================================================
void Panel::DoIdleProcessing( void )
{
	// Nothing to do here for now.
	// If I can help it, I don't want
	// to put anything here.  We should
	// be totally event driven.
}

//=================================================================================
void Panel::TakeSnapShot( void )
{
	if( undoRedo )
	{
		undoRedo->TakeSnapShot( this );
		Frame* frame = wxGetApp().GetFrame();
		frame->UpdateUndoRedoToolBar();
	}
}

//=================================================================================
UndoRedo* Panel::GetUndoRedoSystem( void )
{
	return undoRedo;
}

//=================================================================================
void Panel::SetUndoRedoSystem( UndoRedo* undoRedo, bool deleteCurrent /*= true*/ )
{
	if( this->undoRedo != undoRedo )
	{
		if( this->undoRedo && deleteCurrent )
			delete this->undoRedo;
		this->undoRedo = undoRedo;
	}
}

//=================================================================================
/*virtual*/ void Panel::PreUndoRedoNotice( Cornucopia::Node* newTree, Cornucopia::Node* forgottenTree )
{
}

//=================================================================================
/*virtual*/ void Panel::PostUndoRedoNotice( Cornucopia::Node* oldTree, Cornucopia::Node* forgottenTree )
{
}

//=================================================================================
/*virtual*/ void Panel::PreSave( void )
{
}

//=================================================================================
/*virtual*/ void Panel::PostOpen( void )
{
}

//=================================================================================
/*virtual*/ void Panel::DisplayContextSensativeHelpForNode( Cornucopia::Node* node )
{
	wxString longExplanation = ( const char* )node->GetExplanation( Cornucopia::Node::LONG_EXPLANATION ).c_str();
	if( longExplanation.IsEmpty() )
		longExplanation = wxString::Format( "No lengthy explanation has been set on the node \"%s\" of type \"%s\".", node->GetName().c_str(), node->GetType().c_str() );
	wxMessageBox( longExplanation, wxT( "Long Explanation" ), wxOK | wxCENTRE, this );
}

//=================================================================================
// There can only ever be one node control panel ever highlighted, so here
// we highlight the given panel while unhighlighting all others.  It is okay
// for the given panel to be null, in which case all panels are unhighthlighted.
/*virtual*/ void Panel::HighlightNodeControlPanel( NodeControlPanel* highlightNodeControlPanel )
{
	Cornucopia::Node* root = tree->GetRoot();
	if( !root )
		return;

	Cornucopia::Node::List nodeList;
	root->MakeNodeList( nodeList );
	
	for( Cornucopia::Node::List::iterator iter = nodeList.begin(); iter != nodeList.end(); iter++ )
	{
		Cornucopia::Node* node = *iter;
		EditorMetaData* editorMetaData = ( EditorMetaData* )node->GetMetaData();
		NodeControlPanel* nodeControlPanel = editorMetaData->GetNodeControlPanel();
		if( nodeControlPanel )
		{
			bool highlighted = false;
			if( nodeControlPanel == highlightNodeControlPanel )
				highlighted = true;
			nodeControlPanel->SetHighlighted( highlighted );
		}
	}
}

//=================================================================================
/*virtual*/ bool Panel::FindNext( const wxString& searchCriteria )
{
	wxMessageBox( wxT( "Searching is not yet supported by this note-book panel." ) );
	return false;
}

// Panel.cpp