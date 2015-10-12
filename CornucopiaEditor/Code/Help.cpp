// Help.cpp

#include "CornucopiaEditor.h"

using namespace CornucopiaEditor;

wxIMPLEMENT_DYNAMIC_CLASS( HelpPanel, wxPanel )

//=================================================================================
BEGIN_EVENT_TABLE( HelpPanel, wxPanel )

	EVT_BUTTON( ID_Home, OnHome )
	EVT_BUTTON( ID_Forward, OnForward )
	EVT_BUTTON( ID_Backward, OnBackward )
	EVT_HTML_LINK_CLICKED( wxID_ANY, OnLinkClicked )

END_EVENT_TABLE()

//=================================================================================
HelpPanel::HelpPanel( wxWindow* parent, const wxString& initialPage /*= wxEmptyString*/ ) : wxPanel( parent )
{
	wxPanel* navPanel = new wxPanel( this );

	homeButton = new wxBitmapButton( navPanel, ID_Home, wxArtProvider::GetBitmap( wxART_GO_HOME ) );
	backwardButton = new wxBitmapButton( navPanel, ID_Backward, wxArtProvider::GetBitmap( wxART_GO_BACK ) );
	forwardButton = new wxBitmapButton( navPanel, ID_Forward, wxArtProvider::GetBitmap( wxART_GO_FORWARD ) );

	wxBoxSizer* boxSizer = new wxBoxSizer( wxHORIZONTAL );
	boxSizer->Add( homeButton, 0 );
	boxSizer->Add( backwardButton, 0 );
	boxSizer->Add( forwardButton, 0 );
	navPanel->SetSizer( boxSizer );

	htmlWindow = new wxHtmlWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN );

	if( initialPage.IsEmpty() )
		GoHome();
	else
	{
		GoToPage( initialPage );
		UpdateUI();
	}

	boxSizer = new wxBoxSizer( wxVERTICAL );
	boxSizer->Add( navPanel, 0, wxALL, 5 );
	boxSizer->Add( htmlWindow, 1, wxEXPAND );
	SetSizer( boxSizer );
}

//=================================================================================
/*virtual*/ HelpPanel::~HelpPanel( void )
{
}

//=================================================================================
void HelpPanel::UpdateUI( void )
{
	backwardButton->Enable( htmlWindow->HistoryCanBack() );
	forwardButton->Enable( htmlWindow->HistoryCanForward() );

	NoteBook* noteBook = wxGetApp().GetFrame()->GetNoteBook();
	int pageIndex = noteBook->GetPageIndex( this );
	wxString pageTitle = htmlWindow->GetOpenedPageTitle();
	noteBook->SetPageText( pageIndex, pageTitle );
}

//=================================================================================
// Instead of skipping this event to let the HTML window go to the
// hyperlink target, we have to do it ourselves so that we actually
// add to the HTML window's history list.
void HelpPanel::OnLinkClicked( wxHtmlLinkEvent& event )
{
	const wxHtmlLinkInfo& linkInfo = event.GetLinkInfo();
	wxString reference = linkInfo.GetHref();
	htmlWindow->LoadPage( reference );

	UpdateUI();
}

//=================================================================================
void HelpPanel::OnHome( wxCommandEvent& event )
{
	GoHome();
	UpdateUI();
}

//=================================================================================
void HelpPanel::GoHome( void )
{
	GoToPage( wxGetApp().GetResourceManager()->FormulateFullPath( "Documentation/Index.html" ) );
}

//=================================================================================
void HelpPanel::GoToPage( const wxString& page )
{
	htmlWindow->LoadPage( page );
}

//=================================================================================
void HelpPanel::OnForward( wxCommandEvent& event )
{
	htmlWindow->HistoryForward();
	UpdateUI();
}

//=================================================================================
void HelpPanel::OnBackward( wxCommandEvent& event )
{
	htmlWindow->HistoryBack();
	UpdateUI();
}

// Help.cpp