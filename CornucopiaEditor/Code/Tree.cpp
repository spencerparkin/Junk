// Tree.cpp

#include "CornucopiaEditor.h"

using namespace CornucopiaEditor;

int Tree::nextUntitledNumber = 1;

//=================================================================================
Tree::Tree( void )
{
	root = 0;
	modified = false;
	filePath = wxT( "" );
	untitledNumber = nextUntitledNumber++;
}

//=================================================================================
/*virtual*/ Tree::~Tree( void )
{
	Delete();
}

//=================================================================================
void Tree::Delete( void )
{
	if( root )
		delete root;
	root = 0;
}

//=================================================================================
bool Tree::Open( const wxString& filePath )
{
	// Barf if we already have a tree open.
	if( root )
		return false;

	// No need to watch the current file anymore.  That's about to change.
#if 0
	RemoveAll();
#endif

	// Try to load the given file.
	root = Cornucopia::Node::CreateAndReadFromFile( ( const char* )filePath.c_str(), wxGetApp().GetContext() );
	if( !root )
		return false;

	// Start watching the file if it really exists.
#if 0
	wxFileName fileName( filePath );
	if( fileName.Exists() )
		Add( fileName );
#endif

	// It worked!
	modified = false;
	this->filePath = filePath;
	return true;
}

//=================================================================================
bool Tree::Save( void )
{
	if( !root || filePath.IsEmpty() )
		return false;

	// Assuming that this flag is set correctly, there is
	// nothing for us to do if our tree in memory is not
	// modified from what it is on disk.
	if( !modified )
		return true;

	// Stop watching our file, because we don't want to generate an event caused
	// by us.  We only care about events caused by external applications.
#if 0
	RemoveAll();
#endif

	// Okay, we can now go write the file to disk.
	if( !root->WriteToFile( ( const char* )filePath.c_str(), wxGetApp().GetContext() ) )
		return false;

	// Start watching the file if it now exists.
#if 0
	wxFileName fileName( filePath );
	if( fileName.Exists() )
		Add( fileName );
#endif

	modified = false;
	return true;
}

//=================================================================================
bool Tree::Close( void )
{
	if( !root )
		return false;

	Delete();
	modified = false;
	filePath = wxT( "" );
	return true;
}

//=================================================================================
Cornucopia::Node* Tree::GetRoot( void )
{
	return root;
}

//=================================================================================
bool Tree::SetRoot( Cornucopia::Node* root )
{
	// If one of these is null, we assume that the caller has taken responsibility for the node memory.  If
	// both of these are non-null, then we return false, because we do not want to create a memory leak.
	// This forces the caller to set the root as null before assigning a different root.
	if( root && this->root )
		return false;

	this->root = root;
	modified = true;
	return true;
}

//=================================================================================
bool Tree::GetModified( void )
{
	return modified;
}

//=================================================================================
void Tree::SetModified( bool modified )
{
	this->modified = modified;
}

//=================================================================================
wxString Tree::GetTitle( void )
{
	wxString title = wxString::Format( "Untitled-%d", untitledNumber );

	if( !filePath.IsEmpty() )
	{
		wxFileName fileName = filePath;
		title = fileName.GetName();
	}

	if( modified )
		title += wxT( "*" );

	return title;
}

//=================================================================================
wxString Tree::GetFilePath( void )
{
	return filePath;
}

//=================================================================================
void Tree::SetFilePath( const wxString& filePath )
{
	if( this->filePath != filePath )
	{
		// Stop watching the current file name, if any.
#if 0
		RemoveAll();
#endif

		// Start using the new file name.  (We'll watch it when it is sure to exist on disk.)
		modified = true;
		this->filePath = filePath;
	}
}

//=================================================================================
void Tree::PopulateNonUniqueNameList( std::list< std::string >& nonUniqueNameList )
{
	nonUniqueNameList.clear();

	if( !root )
		return;

	typedef std::map< std::string, int > NameCountMap;
	NameCountMap nameCountMap;
	Cornucopia::Node::List nodeList;
	root->MakeNodeList( nodeList );
	for( Cornucopia::Node::List::iterator iter = nodeList.begin(); iter != nodeList.end(); iter++ )
	{
		Cornucopia::Node* node = *iter;
		std::string nodeName = node->GetName();
		NameCountMap::iterator nameCountIter = nameCountMap.find( nodeName );
		if( nameCountIter == nameCountMap.end() )
			nameCountMap[ nodeName ] = 1;
		else
			nameCountIter->second++;
	}

	for( NameCountMap::iterator nameCountIter = nameCountMap.begin(); nameCountIter != nameCountMap.end(); nameCountIter++ )
	{
		int nameCount = nameCountIter->second;
		if( nameCount > 1 )
			nonUniqueNameList.push_back( nameCountIter->first );
	}
}

// Tree.cpp