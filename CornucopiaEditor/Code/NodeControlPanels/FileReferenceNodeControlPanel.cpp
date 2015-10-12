// FileReferenceNodeControlPanel.cpp

#include "../CornucopiaEditor.h"

using namespace CornucopiaEditor;

//=================================================================================
FileReferenceNodeControlPanel::FileReferenceNodeControlPanel( wxWindow* parent, Cornucopia::Node* node ) : NodeControlPanel( parent, node )
{
	textCtrl = 0;
	openButtonCtrl = 0;
}

//=================================================================================
/*virtual*/ FileReferenceNodeControlPanel::~FileReferenceNodeControlPanel( void )
{
}

//=================================================================================
/*virtual*/ bool FileReferenceNodeControlPanel::PushNodeDataToInterface( void )
{
	Cornucopia::FileReferenceNode* fileRefNode = ( Cornucopia::FileReferenceNode* )node;
	Cornucopia::Path filePath;
	fileRefNode->GetFilePath( filePath );
	std::string filePathString = filePath;

	textCtrl->Clear();
	textCtrl->AppendText( ( const char* )filePathString.c_str() );
	textCtrl->SetInsertionPoint(0);

	return true;
}

//=================================================================================
/*virtual*/ bool FileReferenceNodeControlPanel::PullNodeDataFromInterface( void )
{
	// The button callback will always set the node value.
	return false;
}

//=================================================================================
/*virtual*/ void FileReferenceNodeControlPanel::BuildUserInterface( void )
{
	openButtonCtrl = new wxBitmapButton( this, wxID_ANY, wxArtProvider::GetBitmap( wxART_FILE_OPEN ) );
	textCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_LEFT );

	wxBoxSizer* panelSizer = new wxBoxSizer( wxHORIZONTAL );
	panelSizer->Add( openButtonCtrl, 0, wxALL, 4 );
	panelSizer->Add( textCtrl, 1, wxALL, 4 );
	SetSizer( panelSizer );

	openButtonCtrl->Bind( wxEVT_COMMAND_BUTTON_CLICKED, &FileReferenceNodeControlPanel::OnOpenButtonPressed, this );
}

//=================================================================================
void FileReferenceNodeControlPanel::OnOpenButtonPressed( wxCommandEvent& event )
{
	wxFileDialog fileDialog( wxGetApp().GetFrame() );
	if( fileDialog.ShowModal() == wxID_CANCEL )
		return;

	wxString filePathString = fileDialog.GetPath();
	Cornucopia::Path filePath( ( const char* )filePathString.c_str() );

	Cornucopia::FileReferenceNode* fileRefNode = ( Cornucopia::FileReferenceNode* )node;
	if( fileRefNode->SetFilePath( filePath ) )
	{
		PushNodeDataToInterface();
		SynchronizeTreePanelControls( true );
	}
}

// FileReferenceNodeControlPanel.cpp