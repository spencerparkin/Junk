// BooleanNodeControlPanel.cpp

#include "../CornucopiaEditor.h"

using namespace CornucopiaEditor;

//=================================================================================
BooleanNodeControlPanel::BooleanNodeControlPanel( wxWindow* parent, Cornucopia::Node* node ) : NodeControlPanel( parent, node )
{
}

//=================================================================================
/*virtual*/ BooleanNodeControlPanel::~BooleanNodeControlPanel( void )
{
}

//=================================================================================
/*virtual*/ bool BooleanNodeControlPanel::PushNodeDataToInterface( void )
{
	Cornucopia::BooleanNode* booleanNode = node->Cast< Cornucopia::BooleanNode >();
	if( !booleanNode )
		return false;
	
	checkBox->SetValue( booleanNode->GetBoolean() );
	return true;
}

//=================================================================================
/*virtual*/ bool BooleanNodeControlPanel::PullNodeDataFromInterface( void )
{
	Cornucopia::BooleanNode* booleanNode = node->Cast< Cornucopia::BooleanNode >();
	if( !booleanNode )
		return false;

	booleanNode->SetBoolean( checkBox->GetValue() );
	return true;
}

//=================================================================================
/*virtual*/ void BooleanNodeControlPanel::BuildUserInterface( void )
{
	wxString label = node->GetName();
	checkBox = new wxCheckBox( this, wxID_ANY, label );

	wxBoxSizer* panelSizer = new wxBoxSizer( wxHORIZONTAL );
	panelSizer->Add( checkBox, 1, wxALL, 4 );
	SetSizer( panelSizer );

	checkBox->Bind( wxEVT_COMMAND_CHECKBOX_CLICKED, &BooleanNodeControlPanel::OnCheck, this );
}

//=================================================================================
void BooleanNodeControlPanel::OnCheck( wxCommandEvent& event )
{
	PullNodeDataFromInterface();
	SynchronizeTreePanelControls( true );
}

// BooleanNodeControlPanel.cpp