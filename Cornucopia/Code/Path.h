// Path.h

namespace Cornucopia
{
	//=================================================================================
	// A path may be a path to a directory or a file.
	// It may be a relative path or a fully qualified path.
	// The empty string is considered a valid path, and is the "null path", if you will.
	class CORNUCOPIA_API Path
	{
	public:

		Path( void );
		Path( const Path& path );
		Path( const std::string& path );
		~Path( void );

		Path& operator=( const Path& path );
		operator std::string( void ) const;
		operator const char*( void ) const;

		// Tell us if this and the given path actually refer to the same file on disk.
		bool operator==( const Path& path ) const;

		bool SetPath( const Path& path );
		bool SetPath( const std::string& path );
		bool GetPath( std::string& path, bool lowerCase = false ) const;

		bool IsNull( void ) const;								// Is null if and only if path string is empty.
		bool IsDirectory( void ) const;							// Directories always have a trailing '/'.
		bool IsFile( void ) const;								// Files are not directories.  (Technically, this isn't true on Unix, for example.)
		bool IsFullyQualified( void ) const;					// Full path's always have a ':'.
		bool IsRelative( void ) const;							// Relative paths are not full paths.
		bool Exists( const Path* baseDir = 0 ) const;			// Check that the path exists as a file for directory.
		bool AddBaseDir( const Path& baseDir );					// Convert our relative path into a fully qualified path.
		bool RemoveBaseDir( const Path& baseDir );				// Convert out fully qualified path into a relative path.
		bool GetNameOnly( std::string& fileName ) const;		// Get file name only with extension, or directory name only.
		bool GetExtension( std::string& fileExtention ) const;	// Get file extension, if any.

		static std::string Normalize( const std::string& path );

	private:

		std::string path;
	};
}

// Path.h