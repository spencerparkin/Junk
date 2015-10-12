// LoneSliderControl.cpp

#include "../CornucopiaEditor.h"

using namespace CornucopiaEditor;

//=================================================================================
wxIMPLEMENT_DYNAMIC_CLASS( LoneSliderControl, SliderControlBase );

//=================================================================================
LoneSliderControl::LoneSliderControl( void )
{
	Init();
}

//=================================================================================
/*virtual*/ LoneSliderControl::~LoneSliderControl( void )
{
}

//=================================================================================
LoneSliderControl::LoneSliderControl( wxWindow* parent, wxWindowID winID, int minValue, int maxValue, int value,
					const wxPoint& pos /*= wxDefaultPosition*/, const wxSize& size /*= wxDefaultSize*/,
					long style /*= 0*/, const wxValidator& validator /*= wxDefaultValidator*/,
					const wxString& name /*= wxEmptyString*/ )
{
	Init();
	( void )Create( parent, winID, minValue, maxValue, value, pos, size, style, validator, name );
}

//=================================================================================
bool LoneSliderControl::Create( wxWindow* parent, wxWindowID winID, int minValue, int maxValue, int value,
					const wxPoint& pos /*= wxDefaultPosition*/, const wxSize& size /*= wxDefaultSize*/,
					long style /*= 0*/, const wxValidator& validator /*= wxDefaultValidator*/,
					const wxString& name /*= wxEmptyString*/ )
{
	if( !SliderControlBase::Create( parent, winID, minValue, maxValue, pos, size, style, validator, name ) )
		return false;

	if( !SetValue( value ) )
		return false;

	return true;
}

//=================================================================================
void LoneSliderControl::Init( void )
{
	value = minValue;
}

//=================================================================================
/*virtual*/ void LoneSliderControl::DrawKnobs( wxPaintDC& paintDC, ContinuumGeometry& continuumGeometry )
{
	KnobGeometry knobGeometry;
	CalcKnobGeometry( knobGeometry, value, continuumGeometry );

	if( selectedKnob == THE_KNOB )
		paintDC.SetBrush( *wxGREEN_BRUSH );
	else
		paintDC.SetBrush( *wxRED_BRUSH );
	paintDC.DrawCircle( knobGeometry.position, knobGeometry.radius );
	paintDC.SetBrush( *wxBLACK_BRUSH );
	paintDC.DrawCircle( knobGeometry.position, knobGeometry.radius / 2 );
}

//=================================================================================
/*virtual*/ bool LoneSliderControl::ScrubSelectedKnob( int value )
{
	if( selectedKnob == THE_KNOB && value != this->value && SetValue( value ) )
		return true;
	return false;
}

//=================================================================================
/*virtual*/ bool LoneSliderControl::SetupValueChangedEventTypeAndValue( SliderEvent& event, Knob knob )
{
	if( knob == THE_KNOB )
	{
		event.SetEventType( LONE_SLIDER_EVENT_VALUE_CHANGED );
		event.SetValue( value );
		return true;
	}
	return false;
}

//=================================================================================
/*virtual*/ SliderControlBase::Knob LoneSliderControl::HitTest( const wxPoint& mousePos, const wxSize& size )
{
	ContinuumGeometry continuumGeometry;
	CalcContinuumGeometry( continuumGeometry, size );

	KnobGeometry knobGeometry;
	CalcKnobGeometry( knobGeometry, value, continuumGeometry );

	if( wxPoint2DInt( mousePos - knobGeometry.position ).GetVectorLength() <= wxDouble( knobGeometry.radius ) )
		return THE_KNOB;

	return NO_KNOB;
}

//=================================================================================
int LoneSliderControl::GetValue( void ) const
{
	return value;
}

//=================================================================================
bool LoneSliderControl::SetValue( int value )
{
	if( minValue <= value && value <= maxValue )
	{
		value = SnapValueToTickMark( value );
		this->value = value;
		return true;
	}
	return false;
}

//=================================================================================
/*virtual*/ void LoneSliderControl::ConstrainValuesToRange( void )
{
	if( value < minValue )
		value = minValue;
	if( value > maxValue )
		value = maxValue;
}

// LoneSliderControl.cpp