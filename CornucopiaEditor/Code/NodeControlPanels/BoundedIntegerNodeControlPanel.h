// BoundedIntegerNodeControlPanel.h

namespace CornucopiaEditor
{
	//=================================================================================
	class BoundedIntegerNodeControlPanel : public NodeControlPanel
	{
	public:

		BoundedIntegerNodeControlPanel( wxWindow* parent, Cornucopia::Node* node );
		virtual ~BoundedIntegerNodeControlPanel( void );

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

// BoundedIntegerNodeControlPanel.h