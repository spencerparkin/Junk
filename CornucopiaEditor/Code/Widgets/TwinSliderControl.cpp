// TwinSliderControl.cpp

#include "../CornucopiaEditor.h"

using namespace CornucopiaEditor;

//=================================================================================
wxIMPLEMENT_DYNAMIC_CLASS( TwinSliderControl, SliderControlBase );

//=================================================================================
TwinSliderControl::TwinSliderControl( void )
{
	Init();
}

//=================================================================================
/*virtual*/ TwinSliderControl::~TwinSliderControl( void )
{
}

//=================================================================================
TwinSliderControl::TwinSliderControl( wxWindow* parent, wxWindowID winID, int minValue, int maxValue, int leftValue, int rightValue,
					const wxPoint& pos /*= wxDefaultPosition*/, const wxSize& size /*= wxDefaultSize*/,
					long style /*= 0*/, const wxValidator& validator /*= wxDefaultValidator*/,
					const wxString& name /*= wxEmptyString*/ )
{
	Init();
	( void )Create( parent, winID, minValue, maxValue, leftValue, rightValue, pos, size, style, validator, name );
}

//=================================================================================
bool TwinSliderControl::Create( wxWindow* parent, wxWindowID winID, int minValue, int maxValue, int leftValue, int rightValue,
					const wxPoint& pos /*= wxDefaultPosition*/, const wxSize& size /*= wxDefaultSize*/,
					long style /*= 0*/, const wxValidator& validator /*= wxDefaultValidator*/,
					const wxString& name /*= wxEmptyString*/ )
{
	if( !SliderControlBase::Create( parent, winID, minValue, maxValue, pos, size, style, validator, name ) )
		return false;

	if( !SetValues( leftValue, rightValue ) )
		return false;

	return true;
}

//=================================================================================
void TwinSliderControl::Init( void )
{
	leftValue = minValue;
	rightValue = maxValue;
}

//=================================================================================
/*virtual*/ void TwinSliderControl::DrawKnobs( wxPaintDC& paintDC, ContinuumGeometry& continuumGeometry )
{
	KnobGeometry leftKnobGeometry, rightKnobGeometry;
	CalcKnobGeometry( leftKnobGeometry, leftValue, continuumGeometry );
	CalcKnobGeometry( rightKnobGeometry, rightValue, continuumGeometry );

	wxRect rectClip;
	rectClip.x = leftKnobGeometry.position.x - leftKnobGeometry.radius;
	rectClip.y = leftKnobGeometry.position.y - leftKnobGeometry.radius;
	rectClip.width = leftKnobGeometry.radius;
	rectClip.height = 2 * leftKnobGeometry.radius;

	paintDC.DestroyClippingRegion();
	paintDC.SetClippingRegion( rectClip );
	if( selectedKnob == LEFT_KNOB )
		paintDC.SetBrush( *wxGREEN_BRUSH );
	else
		paintDC.SetBrush( *wxRED_BRUSH );
	paintDC.DrawCircle( leftKnobGeometry.position, leftKnobGeometry.radius );
	paintDC.SetBrush( *wxBLACK_BRUSH );
	paintDC.DrawCircle( leftKnobGeometry.position, leftKnobGeometry.radius / 2 );

	rectClip.x = rightKnobGeometry.position.x;
	rectClip.y = rightKnobGeometry.position.y - rightKnobGeometry.radius;
	rectClip.width = rightKnobGeometry.radius;
	rectClip.height = 2 * rightKnobGeometry.radius;

	paintDC.DestroyClippingRegion();
	paintDC.SetClippingRegion( rectClip );
	if( selectedKnob == RIGHT_KNOB )
		paintDC.SetBrush( *wxGREEN_BRUSH );
	else
		paintDC.SetBrush( *wxRED_BRUSH );
	paintDC.DrawCircle( rightKnobGeometry.position, rightKnobGeometry.radius );
	paintDC.SetBrush( *wxBLACK_BRUSH );
	paintDC.DrawCircle( rightKnobGeometry.position, rightKnobGeometry.radius / 2 );

	paintDC.DestroyClippingRegion();
}

//=================================================================================
/*virtual*/ bool TwinSliderControl::ScrubSelectedKnob( int value )
{
	if( ( selectedKnob == LEFT_KNOB && value != leftValue && SetLeftValue( value ) ) ||
		( selectedKnob == RIGHT_KNOB && value != rightValue && SetRightValue( value ) ) )
	{
		return true;
	}
	return false;
}

//=================================================================================
/*virtual*/ bool TwinSliderControl::SetupValueChangedEventTypeAndValue( SliderEvent& event, Knob knob )
{
	if( knob == LEFT_KNOB )
	{
		event.SetEventType( TWIN_SLIDER_EVENT_LEFT_VALUE_CHANGED );
		event.SetValue( leftValue );
		return true;
	}
	else if( knob == RIGHT_KNOB )
	{
		event.SetEventType( TWIN_SLIDER_EVENT_RIGHT_VALUE_CHANGED );
		event.SetValue( rightValue );
		return true;
	}
	return false;
}

//=================================================================================
/*virtual*/ SliderControlBase::Knob TwinSliderControl::HitTest( const wxPoint& mousePos, const wxSize& size )
{
	ContinuumGeometry continuumGeometry;
	CalcContinuumGeometry( continuumGeometry, size );

	KnobGeometry leftKnobGeometry, rightKnobGeometry;
	CalcKnobGeometry( leftKnobGeometry, leftValue, continuumGeometry );
	CalcKnobGeometry( rightKnobGeometry, rightValue, continuumGeometry );

	if( wxPoint2DInt( mousePos - leftKnobGeometry.position ).GetVectorLength() <= wxDouble( leftKnobGeometry.radius ) )
		if( mousePos.x <= leftKnobGeometry.position.x )
			return LEFT_KNOB;

	if( wxPoint2DInt( mousePos - rightKnobGeometry.position ).GetVectorLength() <= wxDouble( rightKnobGeometry.radius ) )
		if( mousePos.x >= rightKnobGeometry.position.x )
			return RIGHT_KNOB;

	return NO_KNOB;
}

//=================================================================================
int TwinSliderControl::GetLeftValue( void ) const
{
	return leftValue;
}

//=================================================================================
int TwinSliderControl::GetRightValue( void ) const
{
	return rightValue;
}

//=================================================================================
bool TwinSliderControl::SetLeftValue( int leftValue )
{
	if( minValue <= leftValue && leftValue <= rightValue )
	{
		leftValue = SnapValueToTickMark( leftValue );
		this->leftValue = leftValue;
		return true;
	}
	return false;
}

//=================================================================================
bool TwinSliderControl::SetRightValue( int rightValue )
{
	if( leftValue <= rightValue && rightValue <= maxValue )
	{
		rightValue = SnapValueToTickMark( rightValue );
		this->rightValue = rightValue;
		return true;
	}
	return false;
}

//=================================================================================
bool TwinSliderControl::SetValues( int leftValue, int rightValue )
{
	if( minValue <= leftValue && leftValue <= rightValue && rightValue <= maxValue )
	{
		leftValue = SnapValueToTickMark( leftValue );
		rightValue = SnapValueToTickMark( rightValue );
		this->leftValue = leftValue;
		this->rightValue = rightValue;
		return true;
	}
	return false;
}

//=================================================================================
/*virtual*/ void TwinSliderControl::ConstrainValuesToRange( void )
{
	if( leftValue < minValue )
		leftValue = minValue;
	if( rightValue > maxValue )
		rightValue = maxValue;
}

// TwinSliderControl.cpp