// Frame.cpp

#include "Frame.h"
#include "Server.h"
#include "Client.h"
#include "Application.h"
#include "Arbiter.h"
#include <wx/menu.h>
#include <wx/sizer.h>
#include <wx/msgdlg.h>

Frame::Frame( void ) : wxFrame( nullptr, wxID_ANY, "Arbiter" ), timer( this, ID_Timer )
{
	wxMenu* programMenu = new wxMenu();
	wxMenuItem* exitMenuItem = new wxMenuItem( programMenu, ID_Exit, "Exit", "Exit this program." );
	programMenu->Append( exitMenuItem );

	wxMenuBar* menuBar = new wxMenuBar();
	menuBar->Append( programMenu, "Program" );
	SetMenuBar( menuBar );

	wxStatusBar* statusBar = new wxStatusBar( this );
	SetStatusBar( statusBar );

	noteBook = new wxNotebook( this, wxID_ANY );

	wxObjectDataPtr< ClientDataViewModel > clientDataViewModel;
	clientDataView = new wxDataViewCtrl( noteBook, wxID_ANY );
	clientDataViewModel = new ClientDataViewModel();
	clientDataView->AssociateModel( clientDataViewModel.get() );
	clientDataView->AppendColumn( new wxDataViewColumn( "Client", new wxDataViewTextRenderer(), 0 ) );

	noteBook->AddPage( clientDataView, "Clients" );

	ArbiterList& arbiterList = wxGetApp().GetArbiterList();
	for( ArbiterList::iterator iter = arbiterList.begin(); iter != arbiterList.end(); iter++ )
	{
		Arbiter* arbiter = *iter;
		arbiter->AddPageToNoteBook( noteBook );
	}

	wxBoxSizer* boxSizer = new wxBoxSizer( wxVERTICAL );
	boxSizer->Add( noteBook, 1, wxGROW );
	SetSizer( boxSizer );

	Bind( wxEVT_MENU, &Frame::OnExit, this, ID_Exit );
	Bind( wxEVT_TIMER, &Frame::OnTimer, this, ID_Timer );

	inTimer = false;
	timer.Start( 500 );
}

/*virtual*/ Frame::~Frame( void )
{
}

void Frame::OnTimer( wxTimerEvent& event )
{
	if( inTimer )
		return;

	inTimer = true;

	Server* server = wxGetApp().GetServer();
	if( server && !server->Run() )
	{
		timer.Stop();
		wxMessageBox( "Server failed to run.", "Error", wxOK | wxICON_ERROR, this );
	}

	inTimer = false;
}

void Frame::OnExit( wxCommandEvent& event )
{
	Close( true );
}

Frame::ClientDataViewModel::ClientDataViewModel( void )
{
}

/*virtual*/ Frame::ClientDataViewModel::~ClientDataViewModel( void )
{
}

/*virtual*/ bool Frame::ClientDataViewModel::IsContainer( const wxDataViewItem& item ) const
{
	if( !item.IsOk() )
		return true;

	return false;
}

/*virtual*/ wxDataViewItem Frame::ClientDataViewModel::GetParent( const wxDataViewItem& item ) const
{
	wxDataViewItem parentItem;
	return parentItem;
}

/*virtual*/ unsigned int Frame::ClientDataViewModel::GetChildren( const wxDataViewItem& item, wxDataViewItemArray& children ) const
{
	if( item.IsOk() )
		return 0;

	const ClientList& clientList = wxGetApp().GetServer()->GetClientList();

	for( ClientList::const_iterator iter = clientList.cbegin(); iter != clientList.cend(); iter++ )
	{
		const Client* client = *iter;
		wxDataViewItem item;
		item.m_pItem = ( void* )client;
		children.Add( item );
	}

	return clientList.size();
}

/*virtual*/ unsigned int Frame::ClientDataViewModel::GetColumnCount( void ) const
{
	return 1;
}

/*virtual*/ wxString Frame::ClientDataViewModel::GetColumnType( unsigned int col ) const
{
	return "string";
}

/*virtual*/ void Frame::ClientDataViewModel::GetValue( wxVariant& variant, const wxDataViewItem& item, unsigned int col ) const
{
	const Client* client = ( const Client* )item.m_pItem;

	switch( col )
	{
		case 0:
		{
			variant = client->GetName();
			break;
		}
	}
}

/*virtual*/ bool Frame::ClientDataViewModel::SetValue( const wxVariant& variant, const wxDataViewItem& item, unsigned int col )
{
	return false;
}

// Frame.cpp