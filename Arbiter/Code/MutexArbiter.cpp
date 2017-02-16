// MutexArbiter.cpp

#include "MutexArbiter.h"
#include "Client.h"

MutexArbiter::MutexArbiter( void )
{
	dataView = nullptr;
}

/*virtual*/ MutexArbiter::~MutexArbiter( void )
{
}

/*virtual*/ void MutexArbiter::AddPageToNoteBook( wxNotebook* noteBook )
{
	wxObjectDataPtr< DataViewModel > dataViewModel;
	dataView = new wxDataViewCtrl( noteBook, wxID_ANY );
	dataViewModel = new DataViewModel( &mutexPoolMap );
	dataView->AssociateModel( dataViewModel.get() );
	dataView->AppendColumn( new wxDataViewColumn( "Mutex", new wxDataViewTextRenderer(), 0 ) );
	dataView->AppendColumn( new wxDataViewColumn( "Owner", new wxDataViewTextRenderer(), 1 ) );

	noteBook->AddPage( dataView, "Mutices" );
}

/*virtual*/ bool MutexArbiter::CanHandleRequestType( const wxString& requestType )
{
	if( requestType == "AcquireMutex" || requestType == "ReleaseMutex" )
		return true;
	return false;
}

/*virtual*/ void MutexArbiter::HandleRequest( const wxXmlNode* xmlRequestNode, wxXmlNode* xmlResponseNode, const wxString& clientName )
{
	if( clientName.IsEmpty() )
	{
		xmlResponseNode->AddAttribute( "error", "Can't acquire or release a mutex for an unidentified client." );
		return;
	}

	wxString mutexName = Client::GetContent( Client::FindChildNode( xmlRequestNode, "MutexName" ) );

	wxString requestType = Client::GetContent( Client::FindChildNode( xmlRequestNode, "Type" ) );
	if( requestType == "AcquireMutex" )
	{
		bool acquired = false;

		MutexPoolMap::iterator iter = mutexPoolMap.find( mutexName );
		if( iter == mutexPoolMap.end() )
		{
			mutexPoolMap[ mutexName ] = "";
			iter = mutexPoolMap.find( mutexName );
		}

		wxString currentOwnerName = iter->second;
		if( currentOwnerName.IsEmpty() )
		{
			iter->second = clientName;
			acquired = true;
		}

		xmlResponseNode->AddAttribute( "mutexAcquired", ( acquired ? "true" : "false" ) );
	}
	else if( requestType == "ReleaseMutex" )
	{
		bool released = false;

		MutexPoolMap::iterator iter = mutexPoolMap.find( mutexName );
		if( iter == mutexPoolMap.end() )
			xmlResponseNode->AddAttribute( "error", "No such mutex exists: " + mutexName );
		else if( iter->second != clientName )
			xmlResponseNode->AddAttribute( "error", "Mutex not owned by client making release request." );
		else
		{
			iter->second = "";
			released = true;
		}

		xmlResponseNode->AddAttribute( "mutexReleased", ( released ? "true" : "false" ) );
	}
}

/*virtual*/ void MutexArbiter::DroppedClient( const wxString& clientName )
{
	for( MutexPoolMap::iterator iter = mutexPoolMap.begin(); iter != mutexPoolMap.end(); iter++ )
		if( iter->second == clientName )
			iter->second = "";
}

/*virtual*/ void MutexArbiter::UpdateView( void )
{
	dataView->GetModel()->Cleared();
}

MutexArbiter::DataViewModel::DataViewModel( MutexPoolMap* mutexPoolMap )
{
	this->mutexPoolMap = mutexPoolMap;
}

/*virtual*/ MutexArbiter::DataViewModel::~DataViewModel( void )
{
}

/*virtual*/ bool MutexArbiter::DataViewModel::IsContainer( const wxDataViewItem& item ) const
{
	if( !item.IsOk() )
		return true;

	return false;
}

/*virtual*/ wxDataViewItem MutexArbiter::DataViewModel::GetParent( const wxDataViewItem& item ) const
{
	wxDataViewItem parentItem;
	return parentItem;
}

/*virtual*/ unsigned int MutexArbiter::DataViewModel::GetChildren( const wxDataViewItem& item, wxDataViewItemArray& children ) const
{
	if( item.IsOk() )
		return 0;

	keyArray.Clear();

	for( MutexPoolMap::const_iterator iter = mutexPoolMap->begin(); iter != mutexPoolMap->end(); iter++ )
		keyArray.Add( iter->first );

	keyArray.Sort();

	for( int i = 0; i < ( signed )keyArray.GetCount(); i++ )
	{
		wxDataViewItem item;
		item.m_pItem = ( void* )( i + 1 );
		children.Add( item );
	}

	return keyArray.GetCount();
}

/*virtual*/ unsigned int MutexArbiter::DataViewModel::GetColumnCount( void ) const
{
	return 2;
}

/*virtual*/ wxString MutexArbiter::DataViewModel::GetColumnType( unsigned int col ) const
{
	return "string";
}

/*virtual*/ void MutexArbiter::DataViewModel::GetValue( wxVariant& variant, const wxDataViewItem& item, unsigned int col ) const
{
	MutexPoolMap::const_iterator iter = mutexPoolMap->find( keyArray[ ( int )item.GetID() - 1 ] );
	if( iter != mutexPoolMap->end() )
	{
		if( col == 0 )
			variant = iter->first;
		else if( col == 1 )
			variant = iter->second;
	}
}

/*virtual*/ bool MutexArbiter::DataViewModel::SetValue( const wxVariant& variant, const wxDataViewItem& item, unsigned int col )
{
	return false;
}

// MutexArbiter.cpp