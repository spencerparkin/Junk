// SliderControlBase.cpp

#include "../CornucopiaEditor.h"

namespace CornucopiaEditor
{
	//=================================================================================
	wxDEFINE_EVENT( LONE_SLIDER_EVENT_VALUE_CHANGED, SliderEvent );
	wxDEFINE_EVENT( TWIN_SLIDER_EVENT_LEFT_VALUE_CHANGED, SliderEvent );
	wxDEFINE_EVENT( TWIN_SLIDER_EVENT_RIGHT_VALUE_CHANGED, SliderEvent );
}

using namespace CornucopiaEditor;

//=================================================================================
SliderEvent::SliderEvent( int winid /*= 0*/, wxEventType commandType /*= wxEVT_NULL*/ ) : wxEvent( winid, commandType )
{
	value = -1;
	scrubbing = false;
}

//=================================================================================
/*virtual*/ SliderEvent::~SliderEvent( void )
{
}

//=================================================================================
/*virtual*/ wxEvent* SliderEvent::Clone( void ) const
{
	SliderEvent* event = new SliderEvent( GetId(), GetEventType() );
	event->SetValue( value );
	event->SetScrubbing( scrubbing );
	return event;
}

//=================================================================================
wxIMPLEMENT_ABSTRACT_CLASS( SliderControlBase, wxControl );

//=================================================================================
SliderControlBase::SliderControlBase( void )
{
	Init();
}

//=================================================================================
/*virtual*/ SliderControlBase::~SliderControlBase( void )
{
}

//=================================================================================
SliderControlBase::SliderControlBase( wxWindow* parent, wxWindowID winID, int minValue, int maxValue,
							const wxPoint& pos /*= wxDefaultPosition*/, const wxSize& size /*= wxDefaultSize*/,
							long style /*= 0*/, const wxValidator& validator /*= wxDefaultValidator*/,
							const wxString& name /*= wxEmptyString*/ )
{
	Init();
	( void )Create( parent, winID, minValue, maxValue, pos, size, style, validator, name );
}

//=================================================================================
bool SliderControlBase::Create( wxWindow* parent, wxWindowID winID, int minValue, int maxValue,
					const wxPoint& pos /*= wxDefaultPosition*/, const wxSize& size /*= wxDefaultSize*/,
					long style /*= 0*/, const wxValidator& validator /*= wxDefaultValidator*/,
					const wxString& name /*= wxEmptyString*/ )
{
	if( !wxControl::Create( parent, winID, pos, size, style, validator, name ) )
		return false;

	if( !SetRange( minValue, maxValue ) )
		return false;
	
	Bind( wxEVT_PAINT, &SliderControlBase::OnPaint, this );
	Bind( wxEVT_SIZE, &SliderControlBase::OnSize, this );
	Bind( wxEVT_LEFT_DOWN, &SliderControlBase::OnMouseLeftDown, this );
	Bind( wxEVT_LEFT_UP, &SliderControlBase::OnMouseLeftUp, this );
	Bind( wxEVT_MOTION, &SliderControlBase::OnMouseMotion, this );
	Bind( wxEVT_MOUSE_CAPTURE_LOST, &SliderControlBase::OnMouseLostCapture, this );

	return true;
}

//=================================================================================
void SliderControlBase::Init( void )
{
	minValue = 0;
	maxValue = 100;
	tickFrequency = 1;
	selectedKnob = NO_KNOB;
	scrubbing = false;
}

//=================================================================================
/*virtual*/ wxSize SliderControlBase::DoGetBestSize( void ) const
{
	wxSize size = wxControl::DoGetBestSize();
	wxWindow* parent = GetParent();
	if( parent )
		size.SetWidth( parent->GetSize().GetWidth() );
	return size;
}

//=================================================================================
/*virtual*/ wxSize SliderControlBase::DoGetBestClientSize( void ) const
{
	wxSize size = wxControl::DoGetBestClientSize();
	return size;
}

//=================================================================================
void SliderControlBase::CalcContinuumGeometry( ContinuumGeometry& continuumGeometry, const wxSize& size )
{
	continuumGeometry.radius = size.GetHeight() / 2;
	continuumGeometry.leftMostPos.x = continuumGeometry.radius;
	continuumGeometry.leftMostPos.y = size.GetHeight() / 2;
	continuumGeometry.rightMostPos.x = size.GetWidth() - continuumGeometry.radius;
	continuumGeometry.rightMostPos.y = size.GetHeight() / 2;
}

//=================================================================================
void SliderControlBase::CalcKnobGeometry( KnobGeometry& knobGeometry, int value, const ContinuumGeometry& continuumGeometry )
{
	double lerp = double( value - minValue ) / double( maxValue - minValue );
	knobGeometry.position = continuumGeometry.leftMostPos + lerp * ( continuumGeometry.rightMostPos - continuumGeometry.leftMostPos );
	knobGeometry.radius = continuumGeometry.radius;
}

//=================================================================================
void SliderControlBase::OnPaint( wxPaintEvent& event )
{
	wxPaintDC paintDC( this );

	wxSize size = paintDC.GetSize();

	wxRect rectBackground;
	rectBackground.x = 0;
	rectBackground.y = 0;
	rectBackground.width = size.GetWidth();
	rectBackground.height = size.GetHeight();

	paintDC.SetPen( *wxWHITE_PEN );
	paintDC.SetBrush( *wxWHITE_BRUSH );
	paintDC.DrawRectangle( rectBackground );
	paintDC.SetPen( *wxBLACK_PEN );

	ContinuumGeometry continuumGeometry;
	CalcContinuumGeometry( continuumGeometry, size );

	wxRect rectSliderInlet;
	rectSliderInlet.x = continuumGeometry.leftMostPos.x - continuumGeometry.radius;
	rectSliderInlet.y = continuumGeometry.leftMostPos.y - continuumGeometry.radius;
	rectSliderInlet.width = continuumGeometry.rightMostPos.x + continuumGeometry.radius - rectSliderInlet.x;
	rectSliderInlet.height = continuumGeometry.rightMostPos.y + continuumGeometry.radius - rectSliderInlet.y;
	paintDC.DrawRoundedRectangle( rectSliderInlet, continuumGeometry.radius );
	paintDC.DrawLine( continuumGeometry.leftMostPos, continuumGeometry.rightMostPos );

	DrawKnobs( paintDC, continuumGeometry );
}

//=================================================================================
void SliderControlBase::OnMouseLeftDown( wxMouseEvent& event )
{
	wxPoint mousePos = event.GetPosition();
	wxSize size = GetClientSize();
	selectedKnob = HitTest( mousePos, size );
	Refresh();
	CaptureMouse();
}

//=================================================================================
void SliderControlBase::OnMouseLeftUp( wxMouseEvent& event )
{
	ScrubStop();
}

//=================================================================================
void SliderControlBase::OnMouseLostCapture( wxMouseCaptureLostEvent& event )
{
	ScrubStop();
}

//=================================================================================
void SliderControlBase::ScrubStop( void )
{
	if( scrubbing )
	{
		// Not only turn this flag off, but make sure that the event
		// we send tells the handler that we're not scrubbing any more.
		scrubbing = false;
		if( selectedKnob != NO_KNOB )
			SendValueChangedEvent( selectedKnob );
	}

	selectedKnob = NO_KNOB;
	Refresh();
	ReleaseCapture();
}

//=================================================================================
void SliderControlBase::OnMouseMotion( wxMouseEvent& event )
{
	if( event.LeftIsDown() && selectedKnob != NO_KNOB )
	{
		wxPoint mousePos = event.GetPosition();
		wxSize size = GetClientSize();
		
		ContinuumGeometry continuumGeometry;
		CalcContinuumGeometry( continuumGeometry, size );

		double lerp = double( mousePos.x - continuumGeometry.leftMostPos.x ) / double( continuumGeometry.rightMostPos.x - continuumGeometry.leftMostPos.x );
		if( lerp < 0.0 )
			lerp = 0.0;
		if( lerp > 1.0 )
			lerp = 1.0;
		int value = int( double( minValue ) + lerp * double( maxValue - minValue ) );
		value = SnapValueToTickMark( value );

		if( ScrubSelectedKnob( value ) )
		{
			scrubbing = true;
			Refresh();
		}

		if( scrubbing )
			SendValueChangedEvent( selectedKnob );
	}
}

//=================================================================================
void SliderControlBase::SendValueChangedEvent( Knob knob )
{
	SliderEvent event( m_windowId );
	SetupValueChangedEventTypeAndValue( event, knob );
	event.SetEventObject( this );
	event.SetScrubbing( scrubbing );
	event.ResumePropagation( wxEVENT_PROPAGATE_MAX );
	HandleWindowEvent( event );
}

//=================================================================================
void SliderControlBase::OnSize( wxSizeEvent& event )
{
	InvalidateBestSize();
}

//=================================================================================
int SliderControlBase::GetMaxValue( void ) const
{
	return maxValue;
}

//=================================================================================
int SliderControlBase::GetMinValue( void ) const
{
	return minValue;
}

//=================================================================================
bool SliderControlBase::SetRange( int minValue, int maxValue )
{
	if( minValue <= maxValue && ( maxValue - minValue ) % tickFrequency == 0 )
	{
		this->minValue = minValue;
		this->maxValue = maxValue;
		ConstrainValuesToRange();
		return true;
	}
	return false;
}

//=================================================================================
int SliderControlBase::GetTickFrequency( void ) const
{
	return tickFrequency;
}

//=================================================================================
bool SliderControlBase::SetTickFrequency( int tickFrequency )
{
	if( ( maxValue - minValue ) % tickFrequency != 0 )
		return false;

	this->tickFrequency = tickFrequency;
	return true;
}

//=================================================================================
// Here we assume that the given value in our slider's range, and what we do
// is simply snap it to the nearest tick mark based on our tick frequency.
int SliderControlBase::SnapValueToTickMark( int value )
{
	int distanceTraveled = value - minValue;
	int leftTick = value - distanceTraveled % tickFrequency;
	int rightTick = leftTick + tickFrequency;
	int distanceToLeftTick = distanceTraveled - leftTick;
	int distanceToRightTick = rightTick - distanceTraveled;
	if( distanceToLeftTick < distanceToRightTick )
		value = leftTick;
	else
		value = rightTick;
	return value;
}

// SliderControlBase.cpp