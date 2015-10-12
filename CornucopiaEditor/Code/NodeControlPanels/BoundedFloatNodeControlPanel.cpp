// BoundedFloatNodeControlPanel.cpp

#include "../CornucopiaEditor.h"

using namespace CornucopiaEditor;

//=================================================================================
BoundedFloatNodeControlPanel::BoundedFloatNodeControlPanel( wxWindow* parent, Cornucopia::Node* node ) : NodeControlPanel( parent, node )
{
	slider = 0;
	textCtrl = 0;
}

//=================================================================================
/*virtual*/ BoundedFloatNodeControlPanel::~BoundedFloatNodeControlPanel( void )
{
}

//=================================================================================
/*virtual*/ bool BoundedFloatNodeControlPanel::PushNodeDataToInterface( void )
{
	Cornucopia::BoundedFloatNode* boundedFloatNode = ( Cornucopia::BoundedFloatNode* )node;
	Cornucopia::BoundedFloatNode::Data data;
	boundedFloatNode->Get( data );

	double lerp = ( data.value - data.min ) / ( data.max - data.min );
	double sliderMin = slider->GetMin();
	double sliderMax = slider->GetMax();
	double sliderValue = sliderMin + lerp * ( sliderMax - sliderMin );
	int sliderValueInt = int( sliderValue );
	slider->SetValue( sliderValueInt );

	UpdateTextControl();

	return true;
}

//=================================================================================
/*virtual*/ bool BoundedFloatNodeControlPanel::PullNodeDataFromInterface( void )
{
	double sliderMin = slider->GetMin();
	double sliderMax = slider->GetMax();
	double sliderValue = slider->GetValue();
	double lerp = ( sliderValue - sliderMin ) / ( sliderMax - sliderMin );

	Cornucopia::BoundedFloatNode* boundedFloatNode = ( Cornucopia::BoundedFloatNode* )node;
	Cornucopia::BoundedFloatNode::Data data;
	boundedFloatNode->Get( data );
	data.value = data.min + lerp * ( data.max - data.min );
	if( !boundedFloatNode->Set( data ) )
		return false;

	return true;
}

//=================================================================================
void BoundedFloatNodeControlPanel::UpdateTextControl( void )
{
	Cornucopia::BoundedFloatNode* boundedFloatNode = ( Cornucopia::BoundedFloatNode* )node;
	Cornucopia::BoundedFloatNode::Data data;
	boundedFloatNode->Get( data );

	textCtrl->Clear();
	textCtrl->AppendText( wxString::Format( "%1.2f", data.value ) );
}

//=================================================================================
/*virtual*/ void BoundedFloatNodeControlPanel::BuildUserInterface( void )
{
	slider = new wxSlider( this, wxID_ANY, 0, 0, 10000 );
	textCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 40, -1 ), wxTE_PROCESS_ENTER | wxTE_CENTRE );

	wxBoxSizer* panelSizer = new wxBoxSizer( wxHORIZONTAL );
	panelSizer->Add( textCtrl, 0, wxALL, 4 );
	panelSizer->Add( slider, 1, wxALL, 4 );
	SetSizer( panelSizer );

	slider->Bind( wxEVT_SCROLL_THUMBTRACK, &BoundedFloatNodeControlPanel::OnThumbScrub, this );
	slider->Bind( wxEVT_SCROLL_THUMBRELEASE, &BoundedFloatNodeControlPanel::OnThumbRelease, this );
	slider->Bind( wxEVT_SCROLL_PAGEUP, &BoundedFloatNodeControlPanel::OnPageUp, this );
	slider->Bind( wxEVT_SCROLL_PAGEDOWN, &BoundedFloatNodeControlPanel::OnPageDown, this );

	textCtrl->Bind( wxEVT_COMMAND_TEXT_ENTER, &BoundedFloatNodeControlPanel::OnEnter, this );
}

//=================================================================================
void BoundedFloatNodeControlPanel::OnEnter( wxCommandEvent& event )
{
	wxString stringValue = textCtrl->GetValue();
	double value;
	if( !stringValue.ToDouble( &value ) )
		UpdateTextControl();
	else
	{
		Cornucopia::BoundedFloatNode* boundedFloatNode = ( Cornucopia::BoundedFloatNode* )node;
		Cornucopia::BoundedFloatNode::Data data;
		boundedFloatNode->Get( data );
		if( value < data.min )
			value = data.min;
		else if( value > data.max )
			value = data.max;
		data.value = value;
		boundedFloatNode->Set( data );
		PushNodeDataToInterface();
		SynchronizeTreePanelControls( true );
	}
}

//=================================================================================
void BoundedFloatNodeControlPanel::OnThumbScrub( wxScrollEvent& event )
{
	PullNodeDataFromInterface();
	SynchronizeTreePanelControls( false );
	UpdateTextControl();
}

//=================================================================================
void BoundedFloatNodeControlPanel::OnThumbRelease( wxScrollEvent& event )
{
	PullNodeDataFromInterface();
	SynchronizeTreePanelControls( true );
	UpdateTextControl();
}

//=================================================================================
void BoundedFloatNodeControlPanel::OnPageUp( wxScrollEvent& event )
{
	PullNodeDataFromInterface();
	SynchronizeTreePanelControls( true );
	UpdateTextControl();
}

//=================================================================================
void BoundedFloatNodeControlPanel::OnPageDown( wxScrollEvent& event )
{
	PullNodeDataFromInterface();
	SynchronizeTreePanelControls( true );
	UpdateTextControl();
}

// BoundedFloatNodeControlPanel.cpp