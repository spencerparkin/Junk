// BezierCurveNodeControlPanel.h

namespace CornucopiaEditor
{
	//=================================================================================
	class BezierCurveNodeControlPanel : public BoundedPointListNodeControlPanel
	{
		wxDECLARE_DYNAMIC_CLASS( BezierCurveNodeControlPanel );

	public:

		BezierCurveNodeControlPanel( void ) {}	// Can't make this private, but don't use it!
		BezierCurveNodeControlPanel( wxWindow* parent, Cornucopia::Node* node );
		virtual ~BezierCurveNodeControlPanel( void );

		virtual bool PushNodeDataToInterface( void ) override;
		virtual bool PullNodeDataFromInterface( void ) override;

		virtual void CanvasRender( Canvas* canvas, GLenum renderMode ) override;

		virtual wxMenu* CreateMenu( wxEvtHandler* eventHandler ) override;

	protected:

		enum
		{
			ID_ContextMenu_IncDegree = BoundedPointListNodeControlPanel::ID_ContextMenu_Highest,
			ID_ContextMenu_DecDegree,
			ID_ContextMenu_DrawAsParametricFunction,
			ID_ContextMenu_DrawAsFunction,
		};

		virtual void BuildUserInterface( void ) override;

		virtual void OnContextMenu( wxCommandEvent& event ) override;

		enum DrawMode
		{
			DRAW_AS_PARAMETRIC_FUNCTION,
			DRAW_AS_FUNCTION,
		};

		DrawMode drawMode;
		int newtonIters;
	};
}

// BezierCurveNodeControlPanel.h