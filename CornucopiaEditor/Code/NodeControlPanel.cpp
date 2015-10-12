// NodeControlPanel.cpp

#include "CornucopiaEditor.h"

using namespace CornucopiaEditor;

//=================================================================================
BEGIN_EVENT_TABLE( NodeControlPanel, wxPanel )

	EVT_LEFT_DOWN( NodeControlPanel::OnMouse )

END_EVENT_TABLE()

//=================================================================================
wxIMPLEMENT_DYNAMIC_CLASS( NodeControlPanel, wxPanel )

//=================================================================================
HandleSystem< NodeControlPanel > NodeControlPanel::handleSystem;

//=================================================================================
NodeControlPanel::NodeControlPanel( wxWindow* parent, Cornucopia::Node* node ) : wxPanel( parent, wxID_ANY )
{
	this->node = node;
	highlighted = false;

	Bind( wxEVT_HELP, &NodeControlPanel::OnContextSensativeHelp, this );

	handle = handleSystem.CreateHandleFor( this );
}

//=================================================================================
/*virtual*/ NodeControlPanel::~NodeControlPanel( void )
{
	handleSystem.InvalidateHandle( handle );
}

//=================================================================================
int NodeControlPanel::GetHandle( void )
{
	return handle;
}

//=================================================================================
/*static*/ NodeControlPanel* NodeControlPanel::DereferenceHandle( int handle )
{
	return handleSystem.DereferenceHandle( handle );
}

//=================================================================================
void NodeControlPanel::OnContextSensativeHelp( wxHelpEvent& event )
{
	EditorMetaData* editorMetaData = ( EditorMetaData* )node->GetMetaData();
	Panel* panel = editorMetaData->FindAssociatedPanel();
	panel->DisplayContextSensativeHelpForNode( node );
}

//=================================================================================
void NodeControlPanel::SetHighlighted( bool highlighted )
{
	this->highlighted = highlighted;
}

//=================================================================================
bool NodeControlPanel::GetHighlighted( void )
{
	return highlighted;
}

//=================================================================================
/*static*/ NodeControlPanel* NodeControlPanel::CreateForNode( wxWindow* parent, Cornucopia::Node* node )
{
	NodeControlPanel* nodeControlPanel = 0;

	wxString nodeType = node->GetType();

	// These are all the nodes we know about.  If we encounter a node type
	// that we don't know about, then we simply can't provide an interface for it.
	if( nodeType == wxT( "Node" ) )
		nodeControlPanel = new BaseNodeControlPanel( parent, node );
	if( nodeType == wxT( "BooleanNode" ) )
		nodeControlPanel = new BooleanNodeControlPanel( parent, node );
	else if( nodeType == wxT( "StringNode" ) )
		nodeControlPanel = new StringNodeControlPanel( parent, node );
	else if( nodeType == wxT( "BoundedFloatNode" ) )
		nodeControlPanel = new BoundedFloatNodeControlPanel( parent, node );
	else if( nodeType == wxT( "BoundedFloatPairNode" ) )
		nodeControlPanel = new BoundedFloatPairNodeControlPanel( parent, node );
	else if( nodeType == wxT( "BoundedPointListNode" ) )
		nodeControlPanel = new BoundedPointListNodeControlPanel( parent, node );
	else if( nodeType == wxT( "BoundedIntegerNode" ) )
		nodeControlPanel = new BoundedIntegerNodeControlPanel( parent, node );
	else if( nodeType == wxT( "BoundedBoxNode" ) )
		nodeControlPanel = new BoundedBoxNodeControlPanel( parent, node );
	else if( nodeType == wxT( "FileReferenceNode" ) || nodeType == wxT( "OriginReferenceNode" ) )
		nodeControlPanel = new FileReferenceNodeControlPanel( parent, node );
	else if( nodeType == wxT( "VectorE3GANode" ) || nodeType == wxT( "BivectorE3GANode" ) )
		nodeControlPanel = new VecOrBivecNodeControlPanel( parent, node );
	else if( nodeType == wxT( "ConeNode" ) )
		nodeControlPanel = new ConeNodeControlPanel( parent, node );
	else if( nodeType == wxT( "ColorRGBANode" ) )
		nodeControlPanel = new ColorRGBANodeControlPanel( parent, node );
	else if( nodeType == wxT( "SelectionNode" ) )
		nodeControlPanel = new SelectionNodeControlPanel( parent, node );
	else if( nodeType == wxT( "OrientationNode" ) )
		nodeControlPanel = new OrientationNodeControlPanel( parent, node );
	else if( nodeType == wxT( "BezierCurveNode" ) )
		nodeControlPanel = new BezierCurveNodeControlPanel( parent, node );
	else
	{
		// TODO: Defer to plugin?
	}

	if( nodeControlPanel )
		nodeControlPanel->BuildUserInterface();

	return nodeControlPanel;
}

//=================================================================================
/*virtual*/ wxString NodeControlPanel::GetCaption( void )
{
	wxString caption = node->GetName();
	return caption;
}

//=================================================================================
void NodeControlPanel::SynchronizeTreePanelControls( bool updateUndoRedoSystem )
{
	EditorMetaData* editorMetaData = ( EditorMetaData* )node->GetMetaData();
	Panel* panel = editorMetaData->FindAssociatedPanel();
	wxASSERT( panel != 0 );
	if( !panel )
		return;

	// Ask the panel to synchronize any of its controls related to the node.
	panel->SynchronizeControlsWithNode( node );
	
	// This routine is called whenever a node has changed or is being changed by a node control panel.
	// If not asked to update the undo/redo system, it means we're scrubbing a slider.
	if( updateUndoRedoSystem )
		panel->SendLiveTweakEvent( node, false );
	else
		panel->SendLiveTweakEvent( node, true );

	// Let the user (and the system!) know that the tree has been modified.
	Tree* tree = panel->GetTree();
	tree->SetModified( true );
	NoteBook* noteBook = wxGetApp().GetFrame()->GetNoteBook();
	noteBook->UpdateTabTitle( panel );

	// Make sure that the change is also undo-able if told to do so.
	if( updateUndoRedoSystem )
		panel->TakeSnapShot();

	// If we're not already highlighted, we should be.
	Highlight();
}

//=================================================================================
Cornucopia::Node* NodeControlPanel::GetNode( void )
{
	return node;
}

//=================================================================================
void NodeControlPanel::SetNode( Cornucopia::Node* node )
{
	this->node = node;
}

//=================================================================================
/*virtual*/ void NodeControlPanel::CanvasRender( Canvas* canvas, GLenum renderMode )
{
	// The derived class doesn't have to use a canvas widget, but if it
	// does, this method will be called with mode GL_RENDER when it's time
	// to render into it.  For selection, the mode will be GL_SELECT.
}

//=================================================================================
/*virtual*/ void NodeControlPanel::CanvasSelect( Canvas* canvas, unsigned int* hitBuffer, int hitBufferSize, int hitCount )
{
	// The default implementation currently does nothing to process the hit records.
	// The name stack format is up to the derived class, so it's really up to that
	// class to process the hit records.
}

//=================================================================================
/*virtual*/ void NodeControlPanel::OnMouse( wxMouseEvent& event )
{
	Highlight();
}

//=================================================================================
void NodeControlPanel::Highlight( void )
{
	if( !GetHighlighted() )
	{
		EditorMetaData* editorMetaData = ( EditorMetaData* )node->GetMetaData();
		Panel* panel = editorMetaData->FindAssociatedPanel();
		panel->HighlightNodeControlPanel( this );
	}
}

//=================================================================================
/*virtual*/ wxMenu* NodeControlPanel::CreateMenu( wxEvtHandler* eventHandler )
{
	return 0;
}

//=================================================================================
NodeControlPanel::Dialog::Dialog( Cornucopia::Node* node, wxWindow* parent, wxWindowID id,
							const wxString& title, const wxPoint& pos /*= wxDefaultPosition*/, const wxSize& size /*= wxDefaultSize*/ ) :
							wxDialog( parent, id, title, pos, size, wxRESIZE_BORDER | wxCLOSE_BOX | wxCAPTION | wxCLIP_CHILDREN )
{
	nodeControlPanel = CreateForNode( this, node );
	if( nodeControlPanel )
	{
		nodeControlPanel->SetNode( node );
		EditorMetaData* editorMetaData = ( EditorMetaData* )node->GetMetaData();
		editorMetaData->SetNodeControlPanel( nodeControlPanel );
		nodeControlPanel->PushNodeDataToInterface();
	}
}

//=================================================================================
/*virtual*/ NodeControlPanel::Dialog::~Dialog( void )
{
}

//=================================================================================
/*virtual*/ wxWindow* NodeControlPanel::Dialog::GetContentWindow( void ) const
{
	return nodeControlPanel;
}

//=================================================================================
/*virtual*/ bool NodeControlPanel::Dialog::IsModal( void ) const
{
	return false;
}

//=================================================================================
NodeControlPanel* NodeControlPanel::Dialog::GetNodeControlPanel( void )
{
	return nodeControlPanel;
}

// NodeControlPanel.cpp