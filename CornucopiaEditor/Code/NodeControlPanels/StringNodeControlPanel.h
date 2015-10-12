// StringNodeControlPanel.h

namespace CornucopiaEditor
{
	//=================================================================================
	class StringNodeControlPanel : public NodeControlPanel
	{
	public:

		StringNodeControlPanel( wxWindow* parent, Cornucopia::Node* node );
		virtual ~StringNodeControlPanel( void );

		virtual bool PushNodeDataToInterface( void ) override;
		virtual bool PullNodeDataFromInterface( void ) override;

	private:

		void OnEnter( wxCommandEvent& event );

		virtual void BuildUserInterface( void ) override;

		wxTextCtrl* label;
		wxTextCtrl* textCtrl;
	};
}

// StringNodeControlPanel.h