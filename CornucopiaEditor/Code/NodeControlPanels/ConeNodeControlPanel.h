// ConeNodeControlPanel.h

namespace CornucopiaEditor
{
	//=================================================================================
	class ConeNodeControlPanel : public VecOrBivecNodeControlPanel
	{
		wxDECLARE_DYNAMIC_CLASS( ConeNodeControlPanel );

	public:

		ConeNodeControlPanel( void ) {}	// Can't make this private, but don't use it!
		ConeNodeControlPanel( wxWindow* parent, Cornucopia::Node* node );
		virtual ~ConeNodeControlPanel( void );

		virtual bool PushNodeDataToInterface( void ) override;
		virtual bool PullNodeDataFromInterface( void ) override;

	protected:

		wxSlider* coneAngleSlider;
		wxTextCtrl* coneAngleText;

		virtual void BuildUserInterface( void ) override;

		virtual void CanvasRender( Canvas* canvas, GLenum renderMode ) override;

		void DrawCone( Canvas* canvas, int triangleCount );
		void DrawConeSide( int side );
	};
}

// ConeNodeControlPanel.h