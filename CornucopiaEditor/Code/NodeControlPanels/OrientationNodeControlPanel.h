// OrientationNodeControlPanel.h

namespace CornucopiaEditor
{
	//=================================================================================
	class OrientationNodeControlPanel : public NodeControlPanel
	{
		wxDECLARE_DYNAMIC_CLASS( OrientationNodeControlPanel );

	public:

		OrientationNodeControlPanel( void ) {}	// Can't make this private, but don't use it!
		OrientationNodeControlPanel( wxWindow* parent, Cornucopia::Node* node );
		virtual ~OrientationNodeControlPanel( void );

		virtual bool PushNodeDataToInterface( void ) override;
		virtual bool PullNodeDataFromInterface( void ) override;

		virtual void CanvasRender( Canvas* canvas, GLenum renderMode ) override;

	protected:

		enum
		{
			ID_XAngleSlider = wxID_HIGHEST + 1,
			ID_YAngleSlider,
			ID_ZAngleSlider,
		};

		// Scrubbing with the mouse directly on the canvas will offer an
		// alternative to these sliders.
		wxSlider* xAngleSlider;
		wxSlider* yAngleSlider;
		wxSlider* zAngleSlider;

		wxTextCtrl* xAngleText;
		wxTextCtrl* yAngleText;
		wxTextCtrl* zAngleText;

		wxComboBox* eulerAngleOrderComboBox;

		virtual void BuildUserInterface( void ) override;

		void PushNodeToSliders( void );
		void PullNodeFromSliders( void );

		void UpdateAngleText( void );

		void OnComboBoxSelect( wxCommandEvent& event );

		void OnThumbScrub( wxScrollEvent& event );
		void OnThumbRelease( wxScrollEvent& event );
		void OnPageUp( wxScrollEvent& event );
		void OnPageDown( wxScrollEvent& event );
		void OnLineUp( wxScrollEvent& event );
		void OnLineDown( wxScrollEvent& event );

		void ApplyRotation( const c3ga::rotorE3GA& rotation );
		void OnMouseRotationScrubHappening( void );
		void OnMouseRotationScrubFinished( void );

		class OrientationCanvas : public Canvas
		{
		public:

			OrientationCanvas( wxWindow* parent, Canvas::Mode mode );
			virtual ~OrientationCanvas( void );

		protected:

			virtual void OnMouseMove( wxMouseEvent& event ) override;
			virtual void OnMouseLeftDown( wxMouseEvent& event ) override;
			virtual void OnMouseLeftUp( wxMouseEvent& event ) override;
			virtual void OnMouseRightDown( wxMouseEvent& event ) override;

			bool rotationScrubInProgress;
		};

		OrientationCanvas* canvas;

		c3ga::EulerAngleOrder eulerAngleOrder;

		GLuint textureNX, texturePX;
		GLuint textureNY, texturePY;
		GLuint textureNZ, texturePZ;
	};
}

// OrientationNodeControlPanel.h