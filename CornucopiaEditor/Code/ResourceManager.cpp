// ResourceManager.cpp

#include "CornucopiaEditor.h"

using namespace CornucopiaEditor;

//=================================================================================
ResourceManager::ResourceManager( void )
{
}

//=================================================================================
ResourceManager::~ResourceManager( void )
{
}

//=================================================================================
bool ResourceManager::Initialize( int argc, char** argv )
{
	wxImage::AddHandler( new wxPNGHandler() );
	wxFileSystem::AddHandler( new wxArchiveFSHandler() );

	// We expect our resource archive file to exist along-side the executable.
	wxString programPath = wxFileName( argv[0] ).GetPath();
	wxString archivePath = programPath + "\\CornucopiaEditor.zip";		// TODO: Add build step that creates this for us.
	wxFileName fileName( archivePath );
	if( !fileName.Exists() )
		return false;

	// Here's where we can find all our crap.
	basePath = "file:" + archivePath + "#zip:\\Resources\\";
	return true;
}

//=================================================================================
wxString ResourceManager::FormulateFullPath( const wxString& relativePath )
{
	return basePath + relativePath;
}

//=================================================================================
bool ResourceManager::LoadImage( wxImage& image, const wxString& relativePath, wxBitmapType type /*= wxBITMAP_TYPE_ANY*/ )
{
	wxString fullPath = FormulateFullPath( relativePath );
	wxFileSystem fileSystem;
	wxScopedPtr< wxFSFile > filePtr;
	filePtr.reset( fileSystem.OpenFile( fullPath ) );
	if( !filePtr.get() )
		return false;

	wxInputStream* inputStream = filePtr->GetStream();
	if( !inputStream )
		return false;

	return image.LoadFile( *inputStream, type );
}

//=================================================================================
bool ResourceManager::LoadBitmap( wxBitmap& bitmap, const wxString& relativePath, wxBitmapType type /*= wxBITMAP_TYPE_ANY*/ )
{
	wxImage image;
	if( !LoadImage( image, relativePath, type ) )
		return false;

	bitmap = wxBitmap( image );
	return !bitmap.IsNull();
}

// ResourceManager.cpp