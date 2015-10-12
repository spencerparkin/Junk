// ColorRGBANodeControlPanel.h

namespace CornucopiaEditor
{
	//=================================================================================
	class ColorRGBANodeControlPanel : public NodeControlPanel
	{
	public:

		ColorRGBANodeControlPanel( wxWindow* parent, Cornucopia::Node* node );
		virtual ~ColorRGBANodeControlPanel( void );

		virtual bool PushNodeDataToInterface( void ) override;
		virtual bool PullNodeDataFromInterface( void ) override;

	protected:

		virtual void BuildUserInterface( void ) override;

		void OnColorButtonPressed( wxCommandEvent& event );

		void OnThumbScrub( wxScrollEvent& event );
		void OnThumbRelease( wxScrollEvent& event );
		void OnPageUp( wxScrollEvent& event );
		void OnPageDown( wxScrollEvent& event );

		void GetNodeColorRGB( wxColour& color ) const;
		void SetNodeColorRGB( const wxColour& color );
		void GetNodeColorAlpha( wxUint32& alpha ) const;
		void SetNodeColorAlpha( wxUint32 alpha );

		wxColour color;
		wxUint32 alpha;
		wxButton* colorButton;
		wxSlider* alphaSlider;
	};
}

// ColorRGBANodeControlPanel.h