// BoundedFloatPairNodeControlPanel.h

namespace CornucopiaEditor
{
	// TODO: It would be nice if left clicking on a text-box auto-selected all the text.

	//=================================================================================
	// Interfaces similar to this need to be scrub-aware so that we correctly
	// handle the undo/redo system and any live-tweak support.
	class BoundedFloatPairNodeControlPanel : public NodeControlPanel
	{
	public:

		BoundedFloatPairNodeControlPanel( wxWindow* parent, Cornucopia::Node* node );
		virtual ~BoundedFloatPairNodeControlPanel( void );

		virtual bool PushNodeDataToInterface( void ) override;
		virtual bool PullNodeDataFromInterface( void ) override;

	private:

		void OnThumbScrub( wxScrollEvent& event );
		void OnThumbRelease( wxScrollEvent& event );
		void OnPageUp( wxScrollEvent& event );
		void OnPageDown( wxScrollEvent& event );
		void OnKeyDown( wxKeyEvent& event );
		void OnMinEnter( wxCommandEvent& event );
		void OnMaxEnter( wxCommandEvent& event );

		virtual void BuildUserInterface( void ) override;

		void PushRangeToKnob( void );
		void PullRangeFromKnob( void );
		void UpdateTextControls( void );

		enum KnobControlState
		{
			KNOB_CONTROL_MIN,
			KNOB_CONTROL_MAX,
		};

		KnobControlState knobControlState;

		wxSlider* slider;
		wxTextCtrl* minTextCtrl;
		wxTextCtrl* maxTextCtrl;
	};
}

// BoundedFloatPairNodeControlPanel.h