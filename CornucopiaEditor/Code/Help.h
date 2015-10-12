// Help.h

namespace CornucopiaEditor
{
	//=================================================================================
	class HelpPanel : public wxPanel
	{
		wxDECLARE_DYNAMIC_CLASS( HelpPanel );

	public:

		HelpPanel( void ) {}
		HelpPanel( wxWindow* parent, const wxString& initialPage = wxEmptyString );
		virtual ~HelpPanel( void );

		void UpdateUI( void );

		void GoHome( void );
		void GoToPage( const wxString& page );

	private:

		enum
		{
			ID_Home = wxID_HIGHEST,
			ID_Forward,
			ID_Backward,
		};

		void OnLinkClicked( wxHtmlLinkEvent& event );
		void OnHome( wxCommandEvent& event );
		void OnForward( wxCommandEvent& event );
		void OnBackward( wxCommandEvent& event );

		wxBitmapButton* homeButton;
		wxBitmapButton* backwardButton;
		wxBitmapButton* forwardButton;
		wxHtmlWindow* htmlWindow;

		DECLARE_EVENT_TABLE();
	};
}

// Help.h