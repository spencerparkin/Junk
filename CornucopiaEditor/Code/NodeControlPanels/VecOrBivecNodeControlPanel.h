// VecOrBivecNodeControlPanel.h

namespace CornucopiaEditor
{
	//=================================================================================
	// This node control panel can edit a vectorE3GA or bivectorE3GA.
	// The orientation of the element is controlled using spherical coordinates.
	class VecOrBivecNodeControlPanel : public NodeControlPanel
	{
		wxDECLARE_DYNAMIC_CLASS( VecOrBivecNodeControlPanel );

	public:

		VecOrBivecNodeControlPanel( void ) {}	// Can't make this private, but don't use it!
		VecOrBivecNodeControlPanel( wxWindow* parent, Cornucopia::Node* node );
		virtual ~VecOrBivecNodeControlPanel( void );

		virtual bool PushNodeDataToInterface( void ) override;
		virtual bool PullNodeDataFromInterface( void ) override;

		virtual void CanvasRender( Canvas* canvas, GLenum renderMode ) override;

	protected:

		enum
		{
			ID_LongitudeSlider = wxID_HIGHEST + 1,
			ID_LatitudeSlider,
			ID_MagnitudeSlider,
		};

		wxSlider* longitudeSlider;
		wxSlider* latitudeSlider;
		wxSlider* magnitudeSlider;

		wxTextCtrl* longitudeText;
		wxTextCtrl* latitudeText;
		wxTextCtrl* magnitudeText;

		wxTextCtrl* componentText;

		virtual void BuildUserInterface( void ) override;

		void SyncSliderValueLabelsWithSliders( void );
		void SyncComponentTextWithNode( void );

		void OnThumbScrub( wxScrollEvent& event );
		void OnThumbRelease( wxScrollEvent& event );
		void OnPageUp( wxScrollEvent& event );
		void OnPageDown( wxScrollEvent& event );
		void OnLineUp( wxScrollEvent& event );
		void OnLineDown( wxScrollEvent& event );

		static void SphericalCoordinatesFromVector( double& latitudeAngle, double& longitudeAngle, double& magnitude, const c3ga::vectorE3GA& vector );
		static void SphericalCoordinatesToVector( double latitudeAngle, double longitudeAngle, double magnitude, c3ga::vectorE3GA& vector );

		Canvas* canvas;
	};
}

// VecOrBivecNodeControlPanel.h