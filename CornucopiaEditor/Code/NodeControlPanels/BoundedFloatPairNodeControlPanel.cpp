// BoundedFloatPairNodeControlPanel.cpp

#include "../CornucopiaEditor.h"

using namespace CornucopiaEditor;

//=================================================================================
BoundedFloatPairNodeControlPanel::BoundedFloatPairNodeControlPanel( wxWindow* parent, Cornucopia::Node* node ) : NodeControlPanel( parent, node )
{
	slider = 0;
	minTextCtrl = 0;
	maxTextCtrl = 0;
	knobControlState = KNOB_CONTROL_MIN;
}

//=================================================================================
/*virtual*/ BoundedFloatPairNodeControlPanel::~BoundedFloatPairNodeControlPanel( void )
{
}

//=================================================================================
/*virtual*/ bool BoundedFloatPairNodeControlPanel::PushNodeDataToInterface( void )
{
	Cornucopia::BoundedFloatPairNode* boundedFloatPairNode = ( Cornucopia::BoundedFloatPairNode* )node;
	Cornucopia::BoundedFloatPairNode::Data data;
	boundedFloatPairNode->Get( data );

	double minLerp = ( data.minValue - data.min ) / ( data.max - data.min );
	double maxLerp = ( data.maxValue - data.min ) / ( data.max - data.min );
	double sliderMin = slider->GetMin();
	double sliderMax = slider->GetMax();
	double sliderMinValue = sliderMin + minLerp * ( sliderMax - sliderMin );
	double sliderMaxValue = sliderMin + maxLerp * ( sliderMax - sliderMin );
	int sliderMinValueInt = int( sliderMinValue );
	int sliderMaxValueInt = int( sliderMaxValue );
	slider->SetSelection( sliderMinValueInt, sliderMaxValueInt );

	PushRangeToKnob();
	UpdateTextControls();

	return true;
}

//=================================================================================
/*virtual*/ bool BoundedFloatPairNodeControlPanel::PullNodeDataFromInterface( void )
{
	PullRangeFromKnob();

	double sliderMinValue = slider->GetSelStart();
	double sliderMaxValue = slider->GetSelEnd();
	double sliderMin = slider->GetMin();
	double sliderMax = slider->GetMax();
	double sliderMinLerp = ( sliderMinValue - sliderMin ) / ( sliderMax - sliderMin );
	double sliderMaxLerp = ( sliderMaxValue - sliderMin ) / ( sliderMax - sliderMin );

	Cornucopia::BoundedFloatPairNode* boundedFloatPairNode = ( Cornucopia::BoundedFloatPairNode* )node;
	Cornucopia::BoundedFloatPairNode::Data data;
	boundedFloatPairNode->Get( data );
	data.minValue = data.min + sliderMinLerp * ( data.max - data.min );
	data.maxValue = data.min + sliderMaxLerp * ( data.max - data.min );
	if( !boundedFloatPairNode->Set( data ) )
		return false;

	return true;
}

//=================================================================================
/*virtual*/ void BoundedFloatPairNodeControlPanel::BuildUserInterface( void )
{
	slider = new wxSlider( this, wxID_ANY, 0, 0, 10000, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL | wxSL_SELRANGE );
	slider->SetSelection( 0, 0 );

	minTextCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 40, -1 ), wxTE_PROCESS_ENTER | wxTE_CENTRE );
	maxTextCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 40, -1 ), wxTE_PROCESS_ENTER | wxTE_CENTRE );

	wxBoxSizer* panelSizer = new wxBoxSizer( wxHORIZONTAL );
	panelSizer->Add( minTextCtrl, 0, wxALL, 4 );
	panelSizer->Add( slider, 1, wxALL, 4 );
	panelSizer->Add( maxTextCtrl, 0, wxALL, 4 );
	SetSizer( panelSizer );

	slider->Bind( wxEVT_SCROLL_THUMBTRACK, &BoundedFloatPairNodeControlPanel::OnThumbScrub, this );
	slider->Bind( wxEVT_SCROLL_THUMBRELEASE, &BoundedFloatPairNodeControlPanel::OnThumbRelease, this );
	slider->Bind( wxEVT_SCROLL_PAGEUP, &BoundedFloatPairNodeControlPanel::OnPageUp, this );
	slider->Bind( wxEVT_SCROLL_PAGEDOWN, &BoundedFloatPairNodeControlPanel::OnPageDown, this );
	slider->Bind( wxEVT_KEY_DOWN, &BoundedFloatPairNodeControlPanel::OnKeyDown, this );

	minTextCtrl->Bind( wxEVT_COMMAND_TEXT_ENTER, &BoundedFloatPairNodeControlPanel::OnMinEnter, this );
	maxTextCtrl->Bind( wxEVT_COMMAND_TEXT_ENTER, &BoundedFloatPairNodeControlPanel::OnMaxEnter, this );
}

//=================================================================================
void BoundedFloatPairNodeControlPanel::UpdateTextControls( void )
{
	Cornucopia::BoundedFloatPairNode* boundedFloatPairNode = ( Cornucopia::BoundedFloatPairNode* )node;
	Cornucopia::BoundedFloatPairNode::Data data;
	boundedFloatPairNode->Get( data );

	minTextCtrl->Clear();
	minTextCtrl->SetValue( wxString::Format( "%1.2f", data.minValue ) );

	maxTextCtrl->Clear();
	maxTextCtrl->SetValue( wxString::Format( "%1.2f", data.maxValue ) );
}

//=================================================================================
void BoundedFloatPairNodeControlPanel::OnMinEnter( wxCommandEvent& event )
{
	wxString valueString = minTextCtrl->GetValue();
	double value;
	if( !valueString.ToDouble( &value ) )
		UpdateTextControls();
	else
	{
		Cornucopia::BoundedFloatPairNode* boundedFloatPairNode = ( Cornucopia::BoundedFloatPairNode* )node;
		Cornucopia::BoundedFloatPairNode::Data data;
		boundedFloatPairNode->Get( data );
		if( value < data.min )
			value = data.min;
		else if( value > data.maxValue )
			value = data.maxValue;
		data.minValue = value;
		boundedFloatPairNode->Set( data );
		PushNodeDataToInterface();
		SynchronizeTreePanelControls( true );
	}
}

//=================================================================================
void BoundedFloatPairNodeControlPanel::OnMaxEnter( wxCommandEvent& event )
{
	wxString valueString = maxTextCtrl->GetValue();
	double value;
	if( !valueString.ToDouble( &value ) )
		UpdateTextControls();
	else
	{
		Cornucopia::BoundedFloatPairNode* boundedFloatPairNode = ( Cornucopia::BoundedFloatPairNode* )node;
		Cornucopia::BoundedFloatPairNode::Data data;
		boundedFloatPairNode->Get( data );
		if( value > data.max )
			value = data.max;
		else if( value < data.minValue )
			value = data.minValue;
		data.maxValue = value;
		boundedFloatPairNode->Set( data );
		PushNodeDataToInterface();
		SynchronizeTreePanelControls( true );
	}
}

//=================================================================================
void BoundedFloatPairNodeControlPanel::OnThumbScrub( wxScrollEvent& event )
{
	PullNodeDataFromInterface();
	SynchronizeTreePanelControls( false );
	UpdateTextControls();
}

//=================================================================================
void BoundedFloatPairNodeControlPanel::OnThumbRelease( wxScrollEvent& event )
{
	PullNodeDataFromInterface();
	SynchronizeTreePanelControls( true );
	UpdateTextControls();
}

//=================================================================================
void BoundedFloatPairNodeControlPanel::PushRangeToKnob( void )
{
	if( knobControlState == KNOB_CONTROL_MIN )
		slider->SetValue( slider->GetSelStart() );
	else if( knobControlState == KNOB_CONTROL_MAX )
		slider->SetValue( slider->GetSelEnd() );
}

//=================================================================================
void BoundedFloatPairNodeControlPanel::PullRangeFromKnob( void )
{
	if( knobControlState == KNOB_CONTROL_MIN )
	{
		if( slider->GetValue() <= slider->GetSelEnd() )
			slider->SetSelection( slider->GetValue(), slider->GetSelEnd() );
		else
			slider->SetValue( slider->GetSelEnd() );
	}
	else if( knobControlState == KNOB_CONTROL_MAX )
	{
		if( slider->GetValue() >= slider->GetSelStart() )
			slider->SetSelection( slider->GetSelStart(), slider->GetValue() );
		else
			slider->SetValue( slider->GetSelEnd() );
	}
}

//=================================================================================
void BoundedFloatPairNodeControlPanel::OnPageUp( wxScrollEvent& event )
{
	PullNodeDataFromInterface();
	SynchronizeTreePanelControls( true );
	UpdateTextControls();
}

//=================================================================================
void BoundedFloatPairNodeControlPanel::OnPageDown( wxScrollEvent& event )
{
	PullNodeDataFromInterface();
	SynchronizeTreePanelControls( true );
	UpdateTextControls();
}

//=================================================================================
void BoundedFloatPairNodeControlPanel::OnKeyDown( wxKeyEvent& event )
{
	int keyCode = event.GetKeyCode();
	if( keyCode == WXK_LEFT )
		knobControlState = KNOB_CONTROL_MIN;
	else if( keyCode == WXK_RIGHT )
		knobControlState = KNOB_CONTROL_MAX;

	PushRangeToKnob();
}

// BoundedFloatPairNodeControlPanel.cpp