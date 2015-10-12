// BaseNodeControlPanel.cpp

#include "../CornucopiaEditor.h"

using namespace CornucopiaEditor;

//=================================================================================
BaseNodeControlPanel::BaseNodeControlPanel( wxWindow* parent, Cornucopia::Node* node ) : NodeControlPanel( parent, node )
{
}

//=================================================================================
/*virtual*/ BaseNodeControlPanel::~BaseNodeControlPanel( void )
{
}

//=================================================================================
/*virtual*/ bool BaseNodeControlPanel::PushNodeDataToInterface( void )
{
	return true;
}

//=================================================================================
/*virtual*/ bool BaseNodeControlPanel::PullNodeDataFromInterface( void )
{
	return true;
}

//=================================================================================
/*virtual*/ void BaseNodeControlPanel::BuildUserInterface( void )
{
	wxWindow* control = 0;

	wxString explanationTextString = ( const char* )node->GetExplanation( Cornucopia::Node::LONG_EXPLANATION ).c_str();
	if( !explanationTextString.IsEmpty() )
	{
		wxTextCtrl* explanationTextCtrl = new wxTextCtrl( this, wxID_ANY, explanationTextString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_MULTILINE | wxTE_LEFT );
		control = explanationTextCtrl;
	}
	else
	{
		wxStaticLine* staticLine = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHORIZONTAL );
		control = staticLine;
	}

	wxBoxSizer* panelSizer = new wxBoxSizer( wxHORIZONTAL );
	panelSizer->Add( control, 1, wxALL, 0 );
	SetSizer( panelSizer );
}

// BaseNodeControlPanel.cpp