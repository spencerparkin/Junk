// FileReferenceNode.cpp

#include "../Cornucopia.h"

using namespace Cornucopia;

IMPL_CORNUCOPIA_CLASSINFO1( FileReferenceNode, Node );

Path FileReferenceNode::baseDir;

//=================================================================================
FileReferenceNode::FileReferenceNode( void )
{
}

//=================================================================================
/*virtual*/ FileReferenceNode::~FileReferenceNode( void )
{
}

//=================================================================================
/*virtual*/ bool FileReferenceNode::ReadFromTable( lua_State* L, Context& context )
{
	if( !Node::ReadFromTable( L, context ) )
		return context.IssueError( "FileReferenceNode failed to read base-class portion." );

	std::string valueString;
	if( !ReadString( L, context, "fileRef", valueString, "" ) )
		return context.IssueError( "FileReferenceNode \"%s\" failed to read \"fileRef\", because it doesn't exist or isn't a string.", GetName().c_str() );

	if( !filePath.SetPath( valueString ) )
		return context.IssueError( "FileReferenceNode \"%s\" failed to set path \"%s\".", GetName().c_str(), valueString.c_str() );
	
	return true;
}

//=================================================================================
/*virtual*/ bool FileReferenceNode::WriteToTable( lua_State* L, Context& context ) const
{
	if( !Node::WriteToTable( L, context ) )
		return context.IssueError( "FileReferenceNode \"%s\" failed to write base-class portion.", GetName().c_str() );

	std::string stringValue = filePath;
	WriteString( L, context, "fileRef", stringValue );
	return true;
}

//=================================================================================
/*virtual*/ bool FileReferenceNode::Copy( const Node* node, Context& context, const CopyParameters& copyParameters )
{
	if( !Node::Copy( node, context, copyParameters ) )
		return false;

	const FileReferenceNode* fileRefNode = ( const FileReferenceNode* )node;
	filePath = fileRefNode->filePath;
	return true;
}

//=================================================================================
/*static*/ bool FileReferenceNode::SetBaseDir( const Path& baseDir )
{
	if( !baseDir.IsDirectory() )
		return false;

	if( !baseDir.Exists() )
		return false;

	FileReferenceNode::baseDir = baseDir;
	return true;
}

//=================================================================================
/*static*/ bool FileReferenceNode::GetBaseDir( Path& baseDir )
{
	baseDir = FileReferenceNode::baseDir;
	return true;
}

//=================================================================================
bool FileReferenceNode::SetFilePath( const Path& filePath )
{
	Path path = filePath;

	if( !filePath.IsNull() )
	{
		if( !filePath.IsFile() )
			return false;

		if( filePath.IsFullyQualified() )
		{
			if( !filePath.Exists() )
				return false;

			// Strip the base path, (if we have one), so that we store a path that can be completed on any machine.
			if( !baseDir.IsNull() && !path.RemoveBaseDir( baseDir ) )
				return false;
		}
		else	// It's a relative path to a file.
		{
			if( !filePath.Exists( &baseDir ) )
				return false;
		}
	}

	return this->filePath.SetPath( path );
}

//=================================================================================
bool FileReferenceNode::GetFilePath( Path& filePath ) const
{
	return filePath.SetPath( this->filePath );
}

//=================================================================================
/*virtual*/ std::string FileReferenceNode::GetValueToDisplayString( void ) const
{
	std::string fileName;
	filePath.GetNameOnly( fileName );
	return fileName;
}

//=================================================================================
/*virtual*/ Node::ValueStringError FileReferenceNode::GetValueToString( std::string& valueString ) const
{
	valueString = ( const char* )filePath;
	return VSE_NONE;
}

//=================================================================================
/*virtual*/ Node::ValueStringError FileReferenceNode::SetValueFromString( const std::string& valueString )
{
	Path path;
	if( !path.SetPath( valueString ) )
		return VSE_INVALID;
	if( filePath == path )
		return VSE_NO_CHANGE;
	if( !SetFilePath( path ) )
		return VSE_INVALID;
	return VSE_NONE;
}

//=================================================================================
/*virtual*/ Node::ValueStringError FileReferenceNode::GetValueAsStringHelp( std::string& helpString ) const
{
	helpString = "Enter a path to an existing file.";
	return VSE_NONE;
}

// FileReferenceNode.cpp