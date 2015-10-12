// DataObject.cpp

#include "CornucopiaEditor.h"

using namespace CornucopiaEditor;

wxString DataObject::formatString = wxT( "CornucopiaSubTree" );

//=================================================================================
DataObject::DataObject( void )
{
	node = 0;
}

//=================================================================================
/*virtual*/ DataObject::~DataObject( void )
{
}

//=================================================================================
bool DataObject::SetNode( Cornucopia::Node* node )
{
	return node->WriteToFile( "", wxGetApp().GetContext(), &luaBuffer );
}

//=================================================================================
Cornucopia::Node* DataObject::GetNode( void )
{
	return node;
}

//=================================================================================
// We can assume that the given array has as many entries in it as we returned
// from the GetFormatCount method.
/*virtual*/ void DataObject::GetAllFormats( wxDataFormat* formats, Direction dir /*= Get*/ ) const
{
	formats[0].SetId( formatString );
}

//=================================================================================
/*virtual*/ bool DataObject::GetDataHere( const wxDataFormat& format, void* buf ) const
{
	if( format.GetId() != formatString )
		return false;

	if( luaBuffer.empty() )
		return false;

	strcpy_s( ( char* )buf, luaBuffer.size() + 1, luaBuffer.c_str() );
	return true;
}

//=================================================================================
/*virtual*/ size_t DataObject::GetDataSize( const wxDataFormat& format ) const
{
	if( format.GetId() != formatString )
		return 0;

	return luaBuffer.size() + 1;
}

//=================================================================================
/*virtual*/ size_t DataObject::GetFormatCount( Direction dir /*= Get*/ ) const
{
	return 1;
}

//=================================================================================
/*virtual*/ wxDataFormat DataObject::GetPreferredFormat( Direction dir /*= Get*/ ) const
{
	wxDataFormat preferredFormat;
	preferredFormat.SetId( formatString );
	return preferredFormat;
}

//=================================================================================
/*virtual*/ bool DataObject::SetData( const wxDataFormat& format, size_t len, const void* buf )
{
	if( format.GetId() != formatString )
		return false;

	luaBuffer = ( const char* )buf;
	node = Cornucopia::Node::CreateAndReadFromFile( "", wxGetApp().GetContext(), &luaBuffer );
	if( !node )
		return false;

	return true;
}

//=================================================================================
/*static*/ const wxString& DataObject::GetFormatString( void )
{
	return formatString;
}

// DataObject.cpp