// Canvas.h

namespace CornucopiaEditor
{
	//=================================================================================
	// This class is general purpose enough that it could be used anywhere in
	// the application, but it is node-control-panel-aware, meaning that it provides
	// support that specifically cators to derivatives of the NodeControlPanel class.
	class Canvas : public wxGLCanvas
	{
	public:

		enum Mode
		{
			MODE_VIEW_3D,
			MODE_VIEW_2D,
		};

		Canvas( wxWindow* parent, Mode mode );
		virtual ~Canvas( void );

	protected:

		// By default, this will look for an associated node control panel,
		// and then ask it to render into this canvas.
		virtual void Render( GLenum renderMode );

		// By default, mouse events do not call this function.  If the derived
		// class supports selection, it should call this in the overridden
		// mouse events it cares to perform selection on.
		void PerformSelection( wxPoint& mousePos );

		// Override this to always return zero, or something else, if you are using
		// the canvas class in a context other than that of a node control panel.
		virtual NodeControlPanel* GetAssociatedPanel( void );

		virtual void OnErase( wxEraseEvent& event );
		virtual void OnPaint( wxPaintEvent& event );
		virtual void OnResize( wxSizeEvent& event );
		virtual void OnMouseMove( wxMouseEvent& event );
		virtual void OnMouseLeftDown( wxMouseEvent& event );
		virtual void OnMouseLeftUp( wxMouseEvent& event );
		virtual void OnMouseRightDown( wxMouseEvent& event );
		virtual void OnMouseRightUp( wxMouseEvent& event );
		virtual void OnMouseWheelMove( wxMouseEvent& event );
		virtual void OnKeyUp( wxKeyEvent& event );
		virtual void OnKeyDown( wxKeyEvent& event );
		virtual void OnCharHook( wxKeyEvent& event );

		void SetMode( Mode mode );
		Mode GetMode( void );

		void MouseDrag( wxMouseEvent& event, wxPoint* mouseDelta = 0 );

	public:

		// Class derivatives can call these to help render their stuff.
		void Color( float r, float g, float b, float a );
		void RenderVector( const c3ga::vectorE3GA& location, const c3ga::vectorE3GA& vector, double arrowRatio, double arrowRadiusScalar, int arrowSegments );
		void RenderBivector( const c3ga::vectorE3GA& location, const c3ga::bivectorE3GA& bivector, double arrowScale, int arrowCount, int diskSegments );
		void RenderSphere( const c3ga::vectorE3GA& location, double radius, int subDivisionCount = 2 );

		c3ga::vectorE3GA origin;
		c3ga::vectorE3GA unitXAxis;
		c3ga::vectorE3GA unitYAxis;
		c3ga::vectorE3GA unitZAxis;

		struct Camera
		{
			c3ga::vectorE3GA eye, focus, up;
			double foviAngle;
		};

		const Camera& GetCamera( void );

		GLuint CreateTexture( const wxString& textureFile, wxBitmapType type = wxBITMAP_TYPE_ANY );
		void DestroyTexture( GLuint textureName );

	protected:

		bool BindOpenGLContext( void );

		void PrepareForRender( GLenum renderMode, wxPoint* mousePos = 0 );
		void FinishRender( GLenum renderMode );

		void RenderSphereTriangle( const c3ga::vectorE3GA& point0, const c3ga::vectorE3GA& point1, const c3ga::vectorE3GA& point2, const c3ga::evenVersor& evenVersor, int subDivisionCount );

		Mode mode;

		double axisScale;
		double gridScale;
		double subGridScale;
		int gridCount;
		int subGridCount;

		static double ClampedLinearMap( double value, double start, double finish, double v0, double v1 );

		void RenderXYZAxes( void );
		void RenderXYPlaneGrid( double scale, int count, double alphaMultiplier );

		Camera camera;
		wxPoint lastMousePos;

		unsigned int* hitBuffer;
		int hitBufferSize;

		wxGLContext* context;

		DECLARE_EVENT_TABLE()
	};
}

// Canvas.h