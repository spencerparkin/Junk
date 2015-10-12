// SliderControlBase.h

namespace CornucopiaEditor
{
	//=================================================================================
	class SliderEvent : public wxEvent
	{
	public:
		SliderEvent( int winid = 0, wxEventType commandType = wxEVT_NULL );
		virtual ~SliderEvent( void );

		virtual wxEvent* Clone( void ) const override;

		int GetValue( void ) { return value; }
		void SetValue( int value ) { this->value = value; }

		bool GetScrubbing( void ) { return scrubbing; }
		void SetScrubbing( bool scrubbing ) { this->scrubbing = scrubbing; }

	private:
		int value;
		bool scrubbing;
	};

	//=================================================================================
	wxDECLARE_EVENT( LONE_SLIDER_EVENT_VALUE_CHANGED, SliderEvent );
	wxDECLARE_EVENT( TWIN_SLIDER_EVENT_LEFT_VALUE_CHANGED, SliderEvent );
	wxDECLARE_EVENT( TWIN_SLIDER_EVENT_RIGHT_VALUE_CHANGED, SliderEvent );

	//=================================================================================
	// We do embed some knowledge here of the classes we intend to derive from this
	// class, but that's okay for now.
	class SliderControlBase : public wxControl
	{
		wxDECLARE_DYNAMIC_CLASS_NO_COPY( SliderControlBase );

	public:

		SliderControlBase( void );
		virtual ~SliderControlBase( void );

		SliderControlBase( wxWindow* parent, wxWindowID winID, int minValue, int maxValue,
							const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
							long style = 0, const wxValidator& validator = wxDefaultValidator,
							const wxString& name = wxEmptyString );

		bool Create( wxWindow* parent, wxWindowID winID, int minValue, int maxValue,
							const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
							long style = 0, const wxValidator& validator = wxDefaultValidator,
							const wxString& name = wxEmptyString );

		int GetMaxValue( void ) const;
		int GetMinValue( void ) const;
		bool SetRange( int minValue, int maxValue );

		int GetTickFrequency( void ) const;
		bool SetTickFrequency( int tickFrequency );

	protected:

		virtual wxSize DoGetBestSize( void ) const override;
		virtual wxSize DoGetBestClientSize( void ) const override;

		enum Knob
		{
			NO_KNOB,
			THE_KNOB,
			LEFT_KNOB,
			RIGHT_KNOB,
		};

		void Init( void );

		void OnPaint( wxPaintEvent& event );
		void OnSize( wxSizeEvent& event );
		void OnMouseLeftDown( wxMouseEvent& event );
		void OnMouseLeftUp( wxMouseEvent& event );
		void OnMouseMotion( wxMouseEvent& event );
		void OnMouseLostCapture( wxMouseCaptureLostEvent& event );

		struct ContinuumGeometry
		{
			wxPoint leftMostPos;
			wxPoint rightMostPos;
			int radius;
		};

		struct KnobGeometry
		{
			wxPoint position;
			int radius;
		};

		virtual void DrawKnobs( wxPaintDC& paintDC, ContinuumGeometry& continuumGeometry ) = 0;
		virtual Knob HitTest( const wxPoint& mousePos, const wxSize& size ) = 0;
		virtual bool ScrubSelectedKnob( int value ) = 0;
		virtual bool SetupValueChangedEventTypeAndValue( SliderEvent& event, Knob knob ) = 0;
		virtual void ConstrainValuesToRange( void ) = 0;

		void CalcContinuumGeometry( ContinuumGeometry& continuumGeometry, const wxSize& size );
		void CalcKnobGeometry( KnobGeometry& knobGeometry, int value, const ContinuumGeometry& continuumGeometry );

		int SnapValueToTickMark( int value );
		void SendValueChangedEvent( Knob knob );
		void ScrubStop( void );

		int minValue, maxValue;
		int tickFrequency;
		Knob selectedKnob;
		bool scrubbing;
	};
}

// SliderControlBase.h