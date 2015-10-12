// BoundedPointListNodeControlPanel.h

namespace CornucopiaEditor
{
	//=================================================================================
	class BoundedPointListNodeControlPanel : public NodeControlPanel
	{
		wxDECLARE_DYNAMIC_CLASS( BoundedPointListNodeControlPanel );

	public:

		BoundedPointListNodeControlPanel( void ) {}	// Can't make this private, but don't use it!
		BoundedPointListNodeControlPanel( wxWindow* parent, Cornucopia::Node* node );
		virtual ~BoundedPointListNodeControlPanel( void );

		virtual bool PushNodeDataToInterface( void ) override;
		virtual bool PullNodeDataFromInterface( void ) override;

		virtual void CanvasRender( Canvas* canvas, GLenum renderMode ) override;
		virtual void CanvasSelect( Canvas* canvas, unsigned int* hitBuffer, int hitBufferSize, int hitCount ) override;

		virtual wxMenu* CreateMenu( wxEvtHandler* eventHandler ) override;

		bool GetSelectedPoint( c3ga::vectorE2GA& point );
		void PointScrubInProgress( bool pointScrubInProgress );
		bool PointScrubInProgress( void );

	protected:

		enum
		{
			ID_ContextMenu_AddPoint = DataViewCtrl::ID_ContextMenu_Highest,
			ID_ContextMenu_RemovePoint,
			ID_ContextMenu_Bounded,
			ID_ContextMenu_Highest,
		};

		virtual void OnContextMenu( wxCommandEvent& event );

		virtual void BuildUserInterface( void ) override;

		void UpdateCoordinateControls( void );

		virtual void OnXCoordEnter( wxCommandEvent& event );
		virtual void OnYCoordEnter( wxCommandEvent& event );

		void ProcessCoordinateEnter( wxTextCtrl* coordTextCtrl );

		// Derived classes may have additional constraints on how points can be changed, so make it virtual.
		virtual bool ApplyDeltaToSelectedPoint( const c3ga::vectorE2GA& delta );
		virtual void OnScrubHappening( void );
		virtual void OnScrubFinished( void );

		class PointListCanvas : public Canvas
		{
		public:

			PointListCanvas( wxWindow* parent, Canvas::Mode mode );
			virtual ~PointListCanvas( void );

		protected:

			virtual void OnMouseMove( wxMouseEvent& event ) override;
			virtual void OnMouseLeftDown( wxMouseEvent& event ) override;
			virtual void OnMouseLeftUp( wxMouseEvent& event ) override;
			virtual void OnMouseRightDown( wxMouseEvent& event ) override;
			virtual void OnCharHook( wxKeyEvent& event ) override;

			void CenterCameraOnPoints( Cornucopia::BoundedPointListNode* pointListNode );

			bool pointScrubInProgress;
		};

		PointListCanvas* canvas;
		wxTextCtrl* xCoordTextCtrl;
		wxTextCtrl* yCoordTextCtrl;
		wxTextCtrl* indexTextCtrl;

		int selectedPointIndex;
	};
}

// BoundedPointListNodeControlPanel.h