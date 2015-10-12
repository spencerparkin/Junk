// StringNodeControlPanel.cpp

#include "../CornucopiaEditor.h"

using namespace CornucopiaEditor;

//=================================================================================
StringNodeControlPanel::StringNodeControlPanel( wxWindow* parent, Cornucopia::Node* node ) : NodeControlPanel( parent, node )
{
	textCtrl = 0;
	label = 0;
}

//=================================================================================
/*virtual*/ StringNodeControlPanel::~StringNodeControlPanel( void )
{
}

//=================================================================================
/*virtual*/ bool StringNodeControlPanel::PushNodeDataToInterface( void )
{
	Cornucopia::StringNode* stringNode = ( Cornucopia::StringNode* )node;
	std::string stringValue;
	stringNode->GetString( stringValue );

	textCtrl->Clear();
	textCtrl->AppendText( wxString( ( const char* )stringValue.c_str() ) );
	textCtrl->SetInsertionPoint(0);

	label->Clear();
	label->AppendText( wxString( ( const char* )node->GetName().c_str() ) + wxT( ":" ) );

	return true;
}

//=================================================================================
/*virtual*/ bool StringNodeControlPanel::PullNodeDataFromInterface( void )
{
	std::string stringValue = textCtrl->GetValue();

	Cornucopia::StringNode* stringNode = ( Cornucopia::StringNode* )node;
	stringNode->SetString( stringValue );

	return true;
}

//=================================================================================
void StringNodeControlPanel::OnEnter( wxCommandEvent& event )
{
	PullNodeDataFromInterface();
	SynchronizeTreePanelControls( true );
}

//=================================================================================
/*virtual*/ void StringNodeControlPanel::BuildUserInterface( void )
{
	label = new wxTextCtrl( this, wxID_ANY, wxString( ( const char* )node->GetName().c_str() ) + wxT( ":" ), wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_RIGHT );

	// There's no need to use the validator here, but I can see where that may come in handy later on.
	textCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER | wxTE_LEFT );
	textCtrl->SetSize( 80, wxDefaultSize.GetHeight() );

	wxBoxSizer* panelSizer = new wxBoxSizer( wxHORIZONTAL );
	panelSizer->Add( label, 0, wxALL, 4 );
	panelSizer->Add( textCtrl, 1, wxALL, 4 );
	SetSizer( panelSizer );

	textCtrl->Bind( wxEVT_COMMAND_TEXT_ENTER, &StringNodeControlPanel::OnEnter, this );
}

// StringNodeControlPanel.cpp