// Path.cpp

#include "Cornucopia.h"

using namespace Cornucopia;

//=================================================================================
Path::Path( void )
{
	path = "";
}

//=================================================================================
Path::Path( const Path& path )
{
	SetPath( path );
}

//=================================================================================
Path::Path( const std::string& path )
{
	SetPath( path );
}

//=================================================================================
Path::~Path( void )
{
}

//=================================================================================
Path& Path::operator=( const Path& path )
{
	SetPath( path );
	return *this;
}

//=================================================================================
Path::operator std::string( void ) const
{
	return path;
}

//=================================================================================
Path::operator const char*( void ) const
{
	return ( const char* )path.c_str();
}

//=================================================================================
bool Path::SetPath( const Path& path )
{
	return SetPath( path.path );
}

//=================================================================================
bool Path::SetPath( const std::string& path )
{
	this->path = Normalize( path );
	return true;
}

//=================================================================================
bool Path::GetPath( std::string& path, bool lowerCase /*= false*/ ) const
{
	path = this->path;
	if( lowerCase )
		boost::algorithm::to_lower( path );
	return true;
}

//=================================================================================
// The path object always stores it's path in normalized form, so a simple
// case insensative string compare should be enough to determine that two
// paths actually refer to the same file on disk.
bool Path::operator==( const Path& path ) const
{
	std::string thisPathLower = this->path;
	std::string givenPathLower = path.path;

	boost::algorithm::to_lower( thisPathLower );
	boost::algorithm::to_lower( givenPathLower );

	if( thisPathLower == givenPathLower )
		return true;
	return false;
}

//=================================================================================
bool Path::IsNull( void ) const
{
	return path.empty();
}

//=================================================================================
bool Path::IsDirectory( void ) const
{
	if( path.empty() )
		return false;
	if( path[ path.length() - 1 ] == '/' )
		return true;
	return false;
}

//=================================================================================
bool Path::IsFile( void ) const
{
	return !IsDirectory();
}

//=================================================================================
bool Path::IsFullyQualified( void ) const
{
	if( std::string::npos != path.find( ':' ) )
		return true;
	return false;
}

//=================================================================================
bool Path::IsRelative( void ) const
{
	return !IsFullyQualified();
}

//=================================================================================
bool Path::Exists( const Path* baseDir /*= 0*/ ) const
{
	Path fullPath = path;

	if( IsFullyQualified() )
	{
		if( baseDir )
			return false;	// We shouldn't have been given a base path if it is already a full path.
	}
	else	// It's a relative path.
	{
		if( !baseDir )
			return false;	// In this case we cannot complete the relative path.

		if( !fullPath.AddBaseDir( *baseDir ) )
			return false;
	}

	if( IsDirectory() )
	{
		// Do we need to strip the trailing '/' before calling stat?
		struct _stat status;
		_stat( fullPath.path.c_str(), &status );
		if( status.st_mode & S_IFDIR )
			return true;
	}
	else	// It's a file.
	{
		if( -1 != _access( fullPath.path.c_str(), 0 ) )
			return true;
	}

	// It doesn't exist.
	return false;
}

//=================================================================================
bool Path::AddBaseDir( const Path& baseDir )
{
	if( !baseDir.IsFullyQualified() || !baseDir.IsDirectory() || !IsRelative() )
		return false;

	// This should be normalized.
	path = baseDir.path + path;
	return true;
}

//=================================================================================
bool Path::RemoveBaseDir( const Path& baseDir )
{
	if( !baseDir.IsFullyQualified() || !baseDir.IsDirectory() || !IsFullyQualified() )
		return false;

	if( 0 != path.find( baseDir.path ) )
		return false;

	// This should be normalized.
	path.replace( 0, baseDir.path.length() - 1, "" );
	return true;
}

//=================================================================================
bool Path::GetNameOnly( std::string& fileName ) const
{
	fileName = path;
	int index = path.find_last_of( '/' );
	if( index != std::string::npos )
		fileName.replace( 0, index + 1, "" );
	return true;
}

//=================================================================================
bool Path::GetExtension( std::string& fileExtention ) const
{
	if( !GetNameOnly( fileExtention ) )
		return false;
	int index = fileExtention.find_first_of( '.' );
	if( index == std::string::npos )
		return false;
	fileExtention.replace( 0, index + 1, "" );
	boost::algorithm::to_lower( fileExtention );
	return true;
}

//=================================================================================
/*static*/ std::string Path::Normalize( const std::string& path )
{
	std::string normalizedPath = path;

	for( int index = 0; index < ( signed )normalizedPath.length(); index++ )
	{
		if( normalizedPath[ index ] == '\\' )
			normalizedPath[ index ] = '/';
		//else		// Don't change case for now.  Keep this commented out.
		//	normalizedPath[ index ] = tolower( normalizedPath[ index ] );
	}

	// a/b//c/d --> a/b/c/d
	while( true )
	{
		int index = normalizedPath.find( "//" );
		if( index != std::string::npos )
			normalizedPath.replace( index, 2, "/" );
		else
			break;
	}
	
	// a/b/c/../d/e --> a/b/d/e
	while( true )
	{
		int index = normalizedPath.find( "/.." );
		if( index == std::string::npos )
			break;
		else
		{
			int start = normalizedPath.rfind( '/', index - 1 );
			if( start == std::string::npos )
				start = 0;
			else if( ++start == index )
				break;
			int count = index + 4 - start;
			normalizedPath.replace( start, count, "" );
		}
	}

	return normalizedPath;
}

// Path.cpp