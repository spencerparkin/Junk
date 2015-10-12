// TwinSliderControl.h

namespace CornucopiaEditor
{
	//=================================================================================
	// This implements a generic widget that is a slider with two knobs on a single continuum.
	// The left knob is constrained to be left of the right knob, and vice-versa.  This control
	// is useful for authoring sub-ranges of a given range.
	class TwinSliderControl : public SliderControlBase
	{
		wxDECLARE_DYNAMIC_CLASS_NO_COPY( TwinSliderControl );

	public:

		TwinSliderControl( void );
		virtual ~TwinSliderControl( void );

		TwinSliderControl( wxWindow* parent, wxWindowID winID, int minValue, int maxValue, int leftValue, int rightValue,
							const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
							long style = 0, const wxValidator& validator = wxDefaultValidator,
							const wxString& name = wxEmptyString );

		bool Create( wxWindow* parent, wxWindowID winID, int minValue, int maxValue, int leftValue, int rightValue,
							const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
							long style = 0, const wxValidator& validator = wxDefaultValidator,
							const wxString& name = wxEmptyString );

		int GetLeftValue( void ) const;
		int GetRightValue( void ) const;
		bool SetLeftValue( int leftValue );
		bool SetRightValue( int rightValue );
		bool SetValues( int leftValue, int rightValue );

	protected:

		void Init( void );

		virtual void DrawKnobs( wxPaintDC& paintDC, ContinuumGeometry& continuumGeometry ) override;
		virtual Knob HitTest( const wxPoint& mousePos, const wxSize& size ) override;
		virtual bool ScrubSelectedKnob( int value ) override;
		virtual bool SetupValueChangedEventTypeAndValue( SliderEvent& event, Knob knob ) override;
		virtual void ConstrainValuesToRange( void ) override;

		int leftValue, rightValue;
	};
}

// TwinSliderControl.h