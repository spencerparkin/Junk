// BoundedIntegerNodeControlPanel.cpp

#include "../CornucopiaEditor.h"

using namespace CornucopiaEditor;

//=================================================================================
BoundedIntegerNodeControlPanel::BoundedIntegerNodeControlPanel( wxWindow* parent, Cornucopia::Node* node ) : NodeControlPanel( parent, node )
{
	slider = 0;
	textCtrl = 0;
}

//=================================================================================
/*virtual*/ BoundedIntegerNodeControlPanel::~BoundedIntegerNodeControlPanel( void )
{
}

//=================================================================================
/*virtual*/ bool BoundedIntegerNodeControlPanel::PushNodeDataToInterface( void )
{
	Cornucopia::BoundedIntegerNode* boundedIntegerNode = ( Cornucopia::BoundedIntegerNode* )node;
	Cornucopia::BoundedIntegerNode::Data data;
	boundedIntegerNode->Get( data );

	if( !data.IsValid() )
		return false;
	
	int cardinality = data.Cardinality();
	slider->SetMin(0);
	slider->SetMax( cardinality - 1 );
	slider->SetValue( ( data.value - data.min ) / data.multiple );

	UpdateTextControl();

	return true;
}

//=================================================================================
/*virtual*/ bool BoundedIntegerNodeControlPanel::PullNodeDataFromInterface( void )
{
	Cornucopia::BoundedIntegerNode* boundedIntegerNode = ( Cornucopia::BoundedIntegerNode* )node;
	Cornucopia::BoundedIntegerNode::Data data;
	boundedIntegerNode->Get( data );

	data.value = data.min + slider->GetValue() * data.multiple;
	if( !boundedIntegerNode->Set( data ) )
		return false;

	return true;
}

//=================================================================================
void BoundedIntegerNodeControlPanel::UpdateTextControl( void )
{
	Cornucopia::BoundedIntegerNode* boundedIntegerNode = ( Cornucopia::BoundedIntegerNode* )node;
	Cornucopia::BoundedIntegerNode::Data data;
	boundedIntegerNode->Get( data );

	textCtrl->Clear();
	textCtrl->AppendText( wxString::Format( "%d", data.value ) );
}

//=================================================================================
/*virtual*/ void BoundedIntegerNodeControlPanel::BuildUserInterface( void )
{
	slider = new wxSlider( this, wxID_ANY, 0, 0, 1, wxDefaultPosition, wxDefaultSize, wxHORIZONTAL | wxSL_AUTOTICKS );
	textCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 40, -1 ), wxTE_PROCESS_ENTER | wxTE_CENTRE );

	wxBoxSizer* panelSizer = new wxBoxSizer( wxHORIZONTAL );
	panelSizer->Add( textCtrl, 0, wxALL, 4 );
	panelSizer->Add( slider, 1, wxALL, 4 );
	SetSizer( panelSizer );

	slider->Bind( wxEVT_SCROLL_THUMBTRACK, &BoundedIntegerNodeControlPanel::OnThumbScrub, this );
	slider->Bind( wxEVT_SCROLL_THUMBRELEASE, &BoundedIntegerNodeControlPanel::OnThumbRelease, this );
	slider->Bind( wxEVT_SCROLL_PAGEUP, &BoundedIntegerNodeControlPanel::OnPageUp, this );
	slider->Bind( wxEVT_SCROLL_PAGEDOWN, &BoundedIntegerNodeControlPanel::OnPageDown, this );

	textCtrl->Bind( wxEVT_COMMAND_TEXT_ENTER, &BoundedIntegerNodeControlPanel::OnEnter, this );
}

//=================================================================================
void BoundedIntegerNodeControlPanel::OnEnter( wxCommandEvent& event )
{
	wxString stringValue = textCtrl->GetValue();
	long longValue;
	if( !stringValue.ToLong( &longValue ) )
		UpdateTextControl();
	else
	{
		Cornucopia::BoundedIntegerNode* boundedIntegerNode = ( Cornucopia::BoundedIntegerNode* )node;
		Cornucopia::BoundedIntegerNode::Data data;
		boundedIntegerNode->Get( data );
		int value = data.min + Cornucopia::BoundedIntegerNode::RoundToNearestMultiple( int( longValue ) - data.min, data.multiple );
		if( value < data.min )
			value = data.min;
		else if( value > data.max )
			value = data.max;
		data.value = value;
		boundedIntegerNode->Set( data );
		PushNodeDataToInterface();
		SynchronizeTreePanelControls( true );
	}
}

//=================================================================================
void BoundedIntegerNodeControlPanel::OnThumbScrub( wxScrollEvent& event )
{
	PullNodeDataFromInterface();
	SynchronizeTreePanelControls( false );
	UpdateTextControl();
}

//=================================================================================
void BoundedIntegerNodeControlPanel::OnThumbRelease( wxScrollEvent& event )
{
	PullNodeDataFromInterface();
	SynchronizeTreePanelControls( true );
	UpdateTextControl();
}

//=================================================================================
void BoundedIntegerNodeControlPanel::OnPageUp( wxScrollEvent& event )
{
	PullNodeDataFromInterface();
	SynchronizeTreePanelControls( true );
	UpdateTextControl();
}

//=================================================================================
void BoundedIntegerNodeControlPanel::OnPageDown( wxScrollEvent& event )
{
	PullNodeDataFromInterface();
	SynchronizeTreePanelControls( true );
	UpdateTextControl();
}

// BoundedIntegerNodeControlPanel.cpp