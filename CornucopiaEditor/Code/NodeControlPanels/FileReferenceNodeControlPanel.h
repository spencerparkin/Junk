// FileReferenceNodeControlPanel.h

namespace CornucopiaEditor
{
	//=================================================================================
	class FileReferenceNodeControlPanel : public NodeControlPanel
	{
	public:

		FileReferenceNodeControlPanel( wxWindow* parent, Cornucopia::Node* node );
		virtual ~FileReferenceNodeControlPanel( void );

		virtual bool PushNodeDataToInterface( void ) override;
		virtual bool PullNodeDataFromInterface( void ) override;

	private:

		virtual void BuildUserInterface( void ) override;

		void OnOpenButtonPressed( wxCommandEvent& event );

		wxTextCtrl* textCtrl;
		wxBitmapButton* openButtonCtrl;
	};
}

// FileReferenceNodeControlPanel.h