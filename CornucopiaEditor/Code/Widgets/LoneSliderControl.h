// LoneSliderControl.h

namespace CornucopiaEditor
{
	//=================================================================================
	// This implements a generic widget that is a slider with a single knob on a continuum.
	// wxWidgets already supports a slider control, but this one will look similar to the
	// twin slider control, giving the application more consistancy in appearance.
	class LoneSliderControl : public SliderControlBase
	{
		wxDECLARE_DYNAMIC_CLASS_NO_COPY( LoneSliderControl );

	public:

		LoneSliderControl( void );
		virtual ~LoneSliderControl( void );

		LoneSliderControl( wxWindow* parent, wxWindowID winID, int minValue, int maxValue, int value,
							const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
							long style = 0, const wxValidator& validator = wxDefaultValidator,
							const wxString& name = wxEmptyString );

		bool Create( wxWindow* parent, wxWindowID winID, int minValue, int maxValue, int value,
							const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
							long style = 0, const wxValidator& validator = wxDefaultValidator,
							const wxString& name = wxEmptyString );

		int GetValue( void ) const;
		bool SetValue( int value );

	protected:

		void Init( void );

		virtual void DrawKnobs( wxPaintDC& paintDC, ContinuumGeometry& continuumGeometry ) override;
		virtual Knob HitTest( const wxPoint& mousePos, const wxSize& size ) override;
		virtual bool ScrubSelectedKnob( int value ) override;
		virtual bool SetupValueChangedEventTypeAndValue( SliderEvent& event, Knob knob ) override;
		virtual void ConstrainValuesToRange( void ) override;

		int value;
	};
}

// LoneSliderControl.h