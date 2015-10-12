// BoundedFloatNodeControlPanel.h

namespace CornucopiaEditor
{
	//=================================================================================
	// Interfaces similar to this need to be scrub-aware so that we correctly
	// handle the undo/redo system and any live-tweak support.
	class BoundedFloatNodeControlPanel : public NodeControlPanel
	{
	public:

		BoundedFloatNodeControlPanel( wxWindow* parent, Cornucopia::Node* node );
		virtual ~BoundedFloatNodeControlPanel( void );

		virtual bool PushNodeDataToInterface( void ) override;
		virtual bool PullNodeDataFromInterface( void ) override;

	private:

		void OnThumbScrub( wxScrollEvent& event );
		void OnThumbRelease( wxScrollEvent& event );
		void OnPageUp( wxScrollEvent& event );
		void OnPageDown( wxScrollEvent& event );
		void OnEnter( wxCommandEvent& event );

		virtual void BuildUserInterface( void ) override;

		void UpdateTextControl( void );

		wxSlider* slider;
		wxTextCtrl* textCtrl;
	};
}

// BoundedFloatNodeControlPanel.h