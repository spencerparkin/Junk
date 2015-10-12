// BaseNodeControlPanel.h

namespace CornucopiaEditor
{
	//=================================================================================
	class BaseNodeControlPanel : public NodeControlPanel
	{
	public:

		BaseNodeControlPanel( wxWindow* parent, Cornucopia::Node* node );
		virtual ~BaseNodeControlPanel( void );

		virtual bool PushNodeDataToInterface( void ) override;
		virtual bool PullNodeDataFromInterface( void ) override;

	private:

		virtual void BuildUserInterface( void ) override;
	};
}

// BaseNodeControlPanel.h