// BooleanNodeControlPanel.h

namespace CornucopiaEditor
{
	//=================================================================================
	class BooleanNodeControlPanel : public NodeControlPanel
	{
	public:

		BooleanNodeControlPanel( wxWindow* parent, Cornucopia::Node* node );
		virtual ~BooleanNodeControlPanel( void );

		virtual bool PushNodeDataToInterface( void ) override;
		virtual bool PullNodeDataFromInterface( void ) override;

	private:

		void OnCheck( wxCommandEvent& event );

		virtual void BuildUserInterface( void ) override;

		wxCheckBox* checkBox;
	};
}

// BooleanNodeControlPanel.h