// ControlTreePanel.cpp

#include "CornucopiaEditor.h"

using namespace CornucopiaEditor;

wxIMPLEMENT_DYNAMIC_CLASS( ControlTreePanel, Panel )

// TODO: Leaf nodes of the Cornucopia tree don't need a toggle button.
// TODO: Base nodes need some sort of node control panel instead of the placeholder panel.

// TODO: I probably only need one control panel for everything.  Just nest sizers in sizers.

//=================================================================================
BEGIN_EVENT_TABLE( ControlTreePanel, Panel )

	EVT_SIZE( OnSize )

END_EVENT_TABLE()

//=================================================================================
ControlTreePanel::ControlTreePanel( void )
{
}

//=================================================================================
ControlTreePanel::ControlTreePanel( wxWindow* parent, Tree* tree ) : Panel( parent, tree )
{
}

//=================================================================================
/*virtual*/ ControlTreePanel::~ControlTreePanel( void )
{
}

//=================================================================================
/*virtual*/ void ControlTreePanel::BuildUserInterface( void )
{
	RebuildUserInterface();
}

//=================================================================================
void ControlTreePanel::RebuildUserInterface( void )
{
	toggleButtonList.clear();

	// Destroy any pre-existing interface.  This will invalidate
	// all node control panel handles in any current tree.
	DestroyChildren();

	// Go build the new interface as a function of the tree we edit.
	Cornucopia::Node* root = tree->GetRoot();
	if( root )
	{
		wxScrolledWindow* scrolledWindow = new wxScrolledWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL /*| wxALWAYS_SHOW_SB*/ );
		wxBoxSizer* boxSizer = new wxBoxSizer( wxVERTICAL );
		boxSizer->Add( scrolledWindow, 1, wxEXPAND | wxALL, 0 );
		SetSizer( boxSizer );

		BuildUserInterfaceRecursively( root, scrolledWindow );

		scrolledWindow->SetScrollRate( 0, 10 );

		scrolledWindow->Bind( wxEVT_SCROLLWIN_THUMBTRACK, &ControlTreePanel::OnScrollThumbTrack, this );
		scrolledWindow->Bind( wxEVT_SCROLLWIN_THUMBRELEASE, &ControlTreePanel::OnScrollThumbRelease, this );
	}
	else
	{
		wxStaticText* label = new wxStaticText( this, wxID_ANY, wxT( "Empty tree.  Use a different interface to construct a tree.  This interface only supports the editing of nodes in the tree." ) );
		wxBoxSizer* boxSizer = new wxBoxSizer( wxVERTICAL );
		boxSizer->Add( label, 0, wxEXPAND | wxALL, 0 );
		SetSizer( boxSizer );
	}

	RefreshUserInterface();
}

//=================================================================================
void ControlTreePanel::BuildUserInterfaceRecursively( Cornucopia::Node* node, wxWindow* parent )
{
	// Make sure the parent has a sizer.
	wxSizer* parentSizer = parent->GetSizer();
	if( !parentSizer )
	{
		parentSizer = new wxBoxSizer( wxVERTICAL );
		parent->SetSizer( parentSizer );
	}

	// Create the toggle button for this part of the control tree and initialize its state properly.
	EditorMetaData* editorMetaData = ( EditorMetaData* )node->GetMetaData();
	wxString nodeName = ( const char* )node->GetName().c_str();
	ToggleButton* toggleButton = new ToggleButton( parent, node );
	toggleButtonList.push_back( toggleButton );
	toggleButton->SetValue( editorMetaData->GetExpanded() && editorMetaData->GetVisibility() );
	toggleButton->Enable( editorMetaData->GetVisibility() );
	wxSizerFlags sizerFlags;
	sizerFlags.Proportion(0).Border( wxALL, 5 );
	parentSizer->Add( toggleButton, sizerFlags );

	// Create a static box which will surround the contents of this part of the tree.
#ifdef CONTROL_TREE_USE_STATIC_BOX
	wxStaticBox* staticBox = new wxStaticBox( parent, wxID_ANY, wxEmptyString /*nodeName*/ );
#else
	wxPanel* staticBox = new wxPanel( parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_STATIC | wxTAB_TRAVERSAL );
#endif //CONTROL_TREE_USE_STATIC_BOX
	sizerFlags.Proportion(0).Expand();
	if( parent == this )
		sizerFlags.Border( wxALL, 5 );
	else
		sizerFlags.Border( wxLEFT | wxRIGHT | wxUP, 5 );
	parentSizer->Add( staticBox, sizerFlags );
	
	// Create the node control panel that will go inside the static box.
	NodeControlPanel* nodeControlPanel = NodeControlPanel::CreateForNode( staticBox, node );
	editorMetaData->SetNodeControlPanel( nodeControlPanel );
	wxPanel* panel = nodeControlPanel;
	if( !panel )
	{
		// In the absense of a node control panel, create an informative one.
		panel = new wxPanel( staticBox );
		wxStaticText* label = new wxStaticText( panel, wxID_ANY, wxString::Format( "No node control panel for node \"%s\" of type \"%s\".", nodeName.c_str(), ( const char* )node->GetType().c_str() ) );
		wxBoxSizer* panelSizer = new wxBoxSizer( wxVERTICAL );
		panelSizer->Add( label, 0, wxEXPAND | wxALL, 0 );
		panel->SetSizer( panelSizer );
		toggleButton->SetPlaceholderPanel( panel );
	}

	// Create the sizer for the static box.
	wxSizer* staticBoxSizer = new wxBoxSizer( wxVERTICAL );
	sizerFlags.Proportion(0).Expand().Border( wxLEFT | wxRIGHT | wxUP, 10 );
	staticBoxSizer->Add( panel, sizerFlags );
	staticBox->SetSizer( staticBoxSizer );
	
	// Recursively create child controls of the static box using the children of the given node.
	Cornucopia::Node::List* nodeList = node->GetChildren();
	if( nodeList )
	{
		for( Cornucopia::Node::List::iterator iter = nodeList->begin(); iter != nodeList->end(); iter++ )
		{
			Cornucopia::Node* child = *iter;
			BuildUserInterfaceRecursively( child, staticBox );
		}
	}

	// The last static box sizer item needs a full border.
	staticBoxSizer->GetItem( staticBoxSizer->GetItemCount() - 1 )->SetFlag( wxEXPAND | wxALL );

	// Make sure that the initial visibility of the static box is in sync with the toggle button state.
	staticBox->Show( toggleButton->GetValue() );
}

//=================================================================================
/*virtual*/ void ControlTreePanel::RefreshUserInterface( void )
{
	Panel::RefreshUserInterface();

	bool recalcLayout = false;

	for( std::list< ToggleButton* >::iterator iter = toggleButtonList.begin(); iter != toggleButtonList.end(); iter++ )
	{
		ToggleButton* toggleButton = *iter;
		Cornucopia::Node* node = toggleButton->GetNode();
		EditorMetaData* editorMetaData = ( EditorMetaData* )node->GetMetaData();

		// Make sure that the visibility state of each node is reflected in the interface.
		if( editorMetaData->GetVisibility() != toggleButton->IsEnabled() )
		{
			// TODO: The code that follows is currently untested.  Test it.

			toggleButton->Enable( editorMetaData->GetVisibility() );

			if( !toggleButton->IsEnabled() )
			{
				// Each of these should be the same value, but use "or" instead of "and" to be safe.
				if( toggleButton->GetValue() || editorMetaData->GetExpanded() )
				{
					recalcLayout = true;
					editorMetaData->SetExpanded( false );
					toggleButton->SetValue( false );

					NodeControlPanel* nodeControlPanel = editorMetaData->GetNodeControlPanel();
					if( nodeControlPanel )
					{
						wxWindow* parent = nodeControlPanel->GetParent();
#ifdef CONTROL_TREE_USE_STATIC_BOX
						wxStaticBox* staticBox = wxDynamicCast( parent, wxStaticBox );
#else
						wxPanel* staticBox = wxDynamicCast( parent, wxPanel );
#endif //CONTROL_TREE_USE_STATIC_BOX
						if( staticBox )
							staticBox->Show( false );
					}
				}
			}
		}
	}

	if( recalcLayout )
	{
		Layout();
		Refresh();
	}
}

//=================================================================================
void ControlTreePanel::OnSize( wxSizeEvent& event )
{
	event.Skip();
}

//=================================================================================
void ControlTreePanel::OnScrollThumbTrack( wxScrollWinEvent& event )
{
	event.Skip();
}

//=================================================================================
void ControlTreePanel::OnScrollThumbRelease( wxScrollWinEvent& event )
{
	// This is being done in an effort to compensate for a wxWidgets/Aero bug.
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
				nodeControlPanel->Refresh();
		}
	}

	event.Skip();
}

//=================================================================================
/*virtual*/ void ControlTreePanel::PreUndoRedoNotice( Cornucopia::Node* newTree, Cornucopia::Node* forgottenTree )
{
}

//=================================================================================
/*virtual*/ void ControlTreePanel::PostUndoRedoNotice( Cornucopia::Node* oldTree, Cornucopia::Node* forgottenTree )
{
	// If the structure of three remains invarient through the undo/redo movement,
	// we can go ahead and re-use our existing interface instead of having to
	// rebuild the whole thing, which is a bit sluggish and flickery.
	Cornucopia::Node* newTree = tree->GetRoot();
	if( CreateBridge( oldTree, newTree ) )
	{
		// Go patch the toggle button node pointers.
		for( std::list< ToggleButton* >::iterator iter = toggleButtonList.begin(); iter != toggleButtonList.end(); iter++ )
		{
			ToggleButton* toggleButton = *iter;
			Cornucopia::Node* node = toggleButton->GetNode();
			EditorMetaData* editorMetaData = ( EditorMetaData* )node->GetMetaData();
			node = editorMetaData->GetBridgeNode();
			toggleButton->SetNode( node );
		}

		// Go patch the node control panels.  The handles were copied during the cloning
		// process; however, we can't guarentee that the panels referenced in the undo/redo
		// system still exist.  They could be from a different interface style.  Otherwise,
		// we could use the normal node control panel patch routine.  Instead, here we must
		// use the node control panel patch routine that uses the bridge.
		if( oldTree )
			PatchNodeControlPanelsUsingBridge( oldTree );
		
		RefreshUserInterface();
	}
	else
	{
		// Go rebuild the whole UI.
		Freeze();
		RebuildUserInterface();
		Layout();
		Thaw();
	}
}

//=================================================================================
ControlTreePanel::ToggleButton::ToggleButton( wxWindow* parent, Cornucopia::Node* node ) : wxToggleButton( parent, wxID_ANY, ( const char* )node->GetName().c_str(), wxDefaultPosition, wxSize( -1, 20 ) )
{
	this->node = node;
	placeholderPanel = 0;

	Bind( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, &ToggleButton::OnToggle, this );
}

//=================================================================================
/*virtual*/ ControlTreePanel::ToggleButton::~ToggleButton( void )
{
}

//=================================================================================
void ControlTreePanel::ToggleButton::SetNode( Cornucopia::Node* node )
{
	this->node = node;
}

//=================================================================================
Cornucopia::Node* ControlTreePanel::ToggleButton::GetNode( void ) const
{
	return node;
}

//=================================================================================
void ControlTreePanel::ToggleButton::SetPlaceholderPanel( wxPanel* placeholderPanel )
{
	this->placeholderPanel = placeholderPanel;
}

//=================================================================================
wxPanel* ControlTreePanel::ToggleButton::GetPlaceholderPanel( void ) const
{
	return placeholderPanel;
}

//=================================================================================
void ControlTreePanel::ToggleButton::OnToggle( wxCommandEvent& event )
{
	EditorMetaData* editorMetaData = ( EditorMetaData* )node->GetMetaData();
	bool expanded = GetValue();
	editorMetaData->SetExpanded( expanded );

#ifdef CONTROL_TREE_USE_STATIC_BOX
	wxStaticBox* staticBox = 0;
#else
	wxPanel* staticBox = 0;
#endif //CONTROL_TREE_USE_STATIC_BOX

	wxWindow* parent = 0;
	NodeControlPanel* nodeControlPanel = editorMetaData->GetNodeControlPanel();
	if( nodeControlPanel )
		parent = nodeControlPanel->GetParent();
	else if( placeholderPanel )
		parent = placeholderPanel->GetParent();

	if( parent )
	{
#ifdef CONTROL_TREE_USE_STATIC_BOX
		staticBox = wxDynamicCast( parent, wxStaticBox );
#else
		staticBox = wxDynamicCast( parent, wxPanel );
#endif //CONTROL_TREE_USE_STATIC_BOX
	}

	if( staticBox )
	{
		staticBox->Show( editorMetaData->GetExpanded() );
		
		ControlTreePanel* controlTreePanel = FindControlTreePanel();
		if( controlTreePanel )
		{
			controlTreePanel->Layout();
			controlTreePanel->Refresh();
		}
	}
}

//=================================================================================
ControlTreePanel* ControlTreePanel::ToggleButton::FindControlTreePanel( void )
{
	ControlTreePanel* controlTreePanel = 0;
	wxWindow* window = this;
	while( window && !controlTreePanel )
	{
		controlTreePanel = wxDynamicCast( window, ControlTreePanel );
		window = window->GetParent();
	}
	return controlTreePanel;
}

// ControlTreePanel.cpp