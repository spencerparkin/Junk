// SelectionNodeControlPanel.h

namespace CornucopiaEditor
{
	//=================================================================================
	class SelectionNodeControlPanel : public NodeControlPanel
	{
	public:

		SelectionNodeControlPanel( wxWindow* parent, Cornucopia::Node* node );
		virtual ~SelectionNodeControlPanel( void );

		virtual bool PushNodeDataToInterface( void ) override;
		virtual bool PullNodeDataFromInterface( void ) override;

	private:

		virtual void BuildUserInterface( void ) override;

		void OnClick( wxCommandEvent& event );
		void OnDoubleClick( wxCommandEvent& event );

		void RepopulateSelectionBox( void );
		void SyncSelectionBox( void );

		wxListBox* listBox;

		DECLARE_EVENT_TABLE()
	};
}

// SelectionNodeControlPanel.h