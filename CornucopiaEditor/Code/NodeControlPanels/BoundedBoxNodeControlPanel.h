// BoundedBoxNodeControlPanel.h

namespace CornucopiaEditor
{
	//=================================================================================
	// TODO: Render the bounding box with lots of alpha and with the +X,-X,+Y,etc. textures.
	//       To do alpha sorting, render bounding box with right cull mode to do back faces only,
	//       then inner box with right cull mode to do back faces only, then inner box with right
	//       cull mode to do front faces only, then bounding box with right cull mode to do front
	//       faces only.
	//       Use 3 twin slider controls to let the user author the box inside the bounding box.
	class BoundedBoxNodeControlPanel : public NodeControlPanel
	{
		wxDECLARE_DYNAMIC_CLASS( BoundedBoxNodeControlPanel );

	public:

		BoundedBoxNodeControlPanel( void ) {}	// Can't make this private, but don't use it!
		BoundedBoxNodeControlPanel( wxWindow* parent, Cornucopia::Node* node );
		virtual ~BoundedBoxNodeControlPanel( void );

		virtual bool PushNodeDataToInterface( void ) override;
		virtual bool PullNodeDataFromInterface( void ) override;

		virtual void CanvasRender( Canvas* canvas, GLenum renderMode ) override;

	protected:

		virtual void BuildUserInterface( void ) override;

		Canvas* canvas;

		TwinSliderControl* xTwinSlider;
		TwinSliderControl* yTwinSlider;
		TwinSliderControl* zTwinSlider;

		wxTextCtrl* xMinText;
		wxTextCtrl* yMinText;
		wxTextCtrl* zMinText;

		wxTextCtrl* xMaxText;
		wxTextCtrl* yMaxText;
		wxTextCtrl* zMaxText;

		LoneSliderControl* alphaSlider;
	};
}

// BoundedBoxNodeControlPanel.h