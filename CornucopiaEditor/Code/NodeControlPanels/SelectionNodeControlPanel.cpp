// SelectionNodeControlPanel.cpp

#include "../CornucopiaEditor.h"

using namespace CornucopiaEditor;

// TODO: There needs to be a way to switch between single and multi-select mode.

//=================================================================================
BEGIN_EVENT_TABLE( SelectionNodeControlPanel, NodeControlPanel )

	EVT_LISTBOX( wxID_ANY, SelectionNodeControlPanel::OnClick )
	EVT_LISTBOX_DCLICK( wxID_ANY, SelectionNodeControlPanel::OnDoubleClick )

END_EVENT_TABLE()

//=================================================================================
SelectionNodeControlPanel::SelectionNodeControlPanel( wxWindow* parent, Cornucopia::Node* node ) : NodeControlPanel( parent, node )
{
	listBox = 0;
}

//=================================================================================
/*virtual*/ SelectionNodeControlPanel::~SelectionNodeControlPanel( void )
{
}

//=================================================================================
/*virtual*/ bool SelectionNodeControlPanel::PushNodeDataToInterface( void )
{
	RepopulateSelectionBox();
	return true;
}

//=================================================================================
void SelectionNodeControlPanel::RepopulateSelectionBox( void )
{
	Cornucopia::SelectionNode* selectionNode = ( Cornucopia::SelectionNode* )node;
	listBox->Clear();
	for( Cornucopia::SelectionNode::SelectionMap::const_iterator iter = selectionNode->GetSelectionMap().begin(); iter != selectionNode->GetSelectionMap().end(); iter++ )
	{
		wxString itemString = ( const char* )iter->first.c_str();
		int index = listBox->Append( itemString );
		if( iter->second )
			listBox->Select( index );
		else
			listBox->Deselect( index );
	}
}

//=================================================================================
void SelectionNodeControlPanel::SyncSelectionBox( void )
{
	Cornucopia::SelectionNode* selectionNode = ( Cornucopia::SelectionNode* )node;
	for( Cornucopia::SelectionNode::SelectionMap::const_iterator iter = selectionNode->GetSelectionMap().begin(); iter != selectionNode->GetSelectionMap().end(); iter++ )
	{
		wxString itemString = ( const char* )iter->first.c_str();
		int index = listBox->FindString( itemString, true );
		if( index != wxNOT_FOUND )
		{
			if( iter->second )
				listBox->Select( index );
			else
				listBox->Deselect( index );
		}
	}
}

//=================================================================================
/*virtual*/ bool SelectionNodeControlPanel::PullNodeDataFromInterface( void )
{
	// With every user interface event, we manipulate the node, so there is no need to do anything here.
	return true;
}

//=================================================================================
/*virtual*/ void SelectionNodeControlPanel::BuildUserInterface( void )
{
	// Always use multi-select mode for the control.  In single select mode, we will do our own unselecting.
	listBox = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, 0, wxLB_MULTIPLE | wxLB_NEEDED_SB | wxLB_SORT );
	wxBoxSizer* boxSizer = new wxBoxSizer( wxVERTICAL );
	boxSizer->Add( listBox, 1, wxGROW | wxALL, 5 );
	SetSizer( boxSizer );
}

//=================================================================================
void SelectionNodeControlPanel::OnClick( wxCommandEvent& event )
{
	// Undo anything that the control selected or unselected.
	SyncSelectionBox();

	// Which item did they click on?
	int index = event.GetInt();
	if( index == wxNOT_FOUND )
		return;
	std::string itemString = ( const char* )listBox->GetString( index ).c_str();

	// Should we select or unselect the item click on?
	Cornucopia::SelectionNode* selectionNode = ( Cornucopia::SelectionNode* )node;
	bool selected = true;
	if( selectionNode->GetMode() == Cornucopia::SelectionNode::MULTI_SELECT )
		selected = !selectionNode->IsSelected( itemString );
	else if( selectionNode->GetMode() == Cornucopia::SelectionNode::SINGLE_SELECT )
	{
		// If it's already selected, then nothing will change, so don't continue on to add to the undo/redo system.
		if( selectionNode->IsSelected( itemString ) )
			return;
	}
	else
		return;

	// Try to perform the operation.  Did we succeed?
	if( selectionNode->Select( itemString, selected, false ) )
	{
		// Yes.  Synchronize the selection box once again.
		SyncSelectionBox();

		// Make sure the tree panel control matches our panel.
		SynchronizeTreePanelControls( true );
	}
}

//=================================================================================
void SelectionNodeControlPanel::OnDoubleClick( wxCommandEvent& event )
{
	// TODO: Change item name here?
}

// SelectionNodeControlPanel.h