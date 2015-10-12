// Context.cpp

#include "Cornucopia.h"

using namespace Cornucopia;

//=================================================================================
Context::Context( void )
{
	errorList = new std::list< Error* >();
	creationRegistry = 0;
	metaDataCreatorFunc = 0;
	nodeReadingDisposition = NONEXISTENCE_OF_FIELD_IS_FATAL_ERROR;
	autoUpdateOverride = DONT_OVERRIDE_AUTO_UPDATE;
	treeCacheMap = 0;
}

//=================================================================================
/*virtual*/ Context::~Context( void )
{
	ClearAllErrors();
	delete errorList;
	delete creationRegistry;
	WipeTreeCache();
}

//=================================================================================
bool Context::RegisterNodeCreator( const std::string& type, NodeCreatorFunc nodeCreatorFunc )
{
	ClearAllErrors();

	// Create the creation map if we don't already have one.
	if( !creationRegistry )
		creationRegistry = new CreationRegistryMap();

	// Make sure the allocation succeeded.
	if( !creationRegistry )
		return IssueError( "Failed to allocate memory for creation registry." );		// Are we going to fail to allocate the error too?!

	// Make sure that we don't stop an already-registered node type.
	NodeCreatorFunc existingNodeCreatorFunc = ( *creationRegistry )[ type ];
	if( existingNodeCreatorFunc )
		return IssueError( "Failed to register creator for type \"%s\", because an existing creator has already been registered.", type.c_str() );

	// Register the node type with a creator function.
	( *creationRegistry )[ type ] = nodeCreatorFunc;
	return true;
}

//=================================================================================
Context::NodeCreatorFunc Context::UnregisterNodeCreator( const std::string& type )
{
	ClearAllErrors();

	// If we've never registered, we can't unregister anything.
	if( !creationRegistry )
	{
		IssueError( "Failed to unregister creator for type \"%s\", because the creation registry is null.", type.c_str() );
		return 0;
	}

	// Remove the regsitered creator for the given type and return the creator function for the caller's reference.
	NodeCreatorFunc existingNodeCreatorFunc = ( *creationRegistry )[ type ];
	( *creationRegistry )[ type ] = 0;
	return existingNodeCreatorFunc;
}

//=================================================================================
Context::NodeCreatorFunc Context::LookupNodeCreator( const std::string& type )
{
	ClearAllErrors();

	// If we've never registered, there's nothing to look up!
	if( !creationRegistry )
	{
		IssueError( "Failed to look-up creator for type \"%s\", because the creation registry is null.", type.c_str() );
		return 0;
	}

	NodeCreatorFunc nodeCreatorFunc = ( *creationRegistry )[ type ];
	return nodeCreatorFunc;
}

//=================================================================================
// Register the built-in nodes types supported by Cornucopia.
// Users of the library can certainly add their own types.
bool Context::RegisterAllBuiltinNodeCreators( void )
{
	RegisterNodeCreator( Node::classInfo.Type(),					( NodeCreatorFunc )&Node::Create );
	RegisterNodeCreator( BooleanNode::classInfo.Type(),				( NodeCreatorFunc )&BooleanNode::Create );
	RegisterNodeCreator( BoundedFloatNode::classInfo.Type(),		( NodeCreatorFunc )&BoundedFloatNode::Create );
	RegisterNodeCreator( BoundedFloatPairNode::classInfo.Type(),	( NodeCreatorFunc )&BoundedFloatPairNode::Create );
	RegisterNodeCreator( BoundedBoxNode::classInfo.Type(),			( NodeCreatorFunc )&BoundedBoxNode::Create );
	RegisterNodeCreator( StringNode::classInfo.Type(),				( NodeCreatorFunc )&StringNode::Create );
	RegisterNodeCreator( FileReferenceNode::classInfo.Type(),		( NodeCreatorFunc )&FileReferenceNode::Create );
	RegisterNodeCreator( VectorE3GANode::classInfo.Type(),			( NodeCreatorFunc )&VectorE3GANode::Create );
	RegisterNodeCreator( BivectorE3GANode::classInfo.Type(),		( NodeCreatorFunc )&BivectorE3GANode::Create );
	RegisterNodeCreator( ConeNode::classInfo.Type(),				( NodeCreatorFunc )&ConeNode::Create );
	RegisterNodeCreator( ColorRGBANode::classInfo.Type(),			( NodeCreatorFunc )&ColorRGBANode::Create );
	RegisterNodeCreator( OriginReferenceNode::classInfo.Type(),		( NodeCreatorFunc )&OriginReferenceNode::Create );
	RegisterNodeCreator( BoundedPointListNode::classInfo.Type(),	( NodeCreatorFunc )&BoundedPointListNode::Create );
	RegisterNodeCreator( SelectionNode::classInfo.Type(),			( NodeCreatorFunc )&SelectionNode::Create );
	RegisterNodeCreator( OrientationNode::classInfo.Type(),			( NodeCreatorFunc )&OrientationNode::Create );
	RegisterNodeCreator( BezierCurveNode::classInfo.Type(),			( NodeCreatorFunc )&BezierCurveNode::Create );
	RegisterNodeCreator( BoundedIntegerNode::classInfo.Type(),		( NodeCreatorFunc )&BoundedIntegerNode::Create );

	if( errorList->size() > 0 )
		return false;

	return true;
}

//=================================================================================
void Context::SetMetaDataCreatorFunc( MetaDataCreatorFunc metaDataCreatorFunc )
{
	this->metaDataCreatorFunc = metaDataCreatorFunc;
}

//=================================================================================
Context::MetaDataCreatorFunc Context::GetMetaDataCreatorFunc( void )
{
	return metaDataCreatorFunc;
}

//=================================================================================
void Context::MakeListOfAllRegisteredNodeTypes( std::list< std::string >& nodeTypeList, bool sort /*= true*/ )
{
	nodeTypeList.clear();
	if( creationRegistry )
		for( CreationRegistryMap::iterator iter = creationRegistry->begin(); iter != creationRegistry->end(); iter++ )
			nodeTypeList.push_back( iter->first );

	if( sort )
		nodeTypeList.sort();
}

//=================================================================================
Node* Context::CreateNode( const std::string& type )
{
	Node* node = 0;
	bool success = false;

	try
	{
		// Do we know about this node type?
		Context::NodeCreatorFunc nodeCreatorFunc = LookupNodeCreator( type );
		if( !nodeCreatorFunc )
			throw new Error( "Failed to create node, because the node type \"%s\" was not registered with the system.", type.c_str() );

		// Try to create the node type.
		node = ( *nodeCreatorFunc )();
		if( !node )
			throw new Error( "The node creator function for type \"%s\" failed to create a node.", type.c_str() );

		// Create meta-data for the node if the user registered a meta-data creator function.
		Context::MetaDataCreatorFunc metaDataCreatorFunc = GetMetaDataCreatorFunc();
		if( metaDataCreatorFunc )
		{
			MetaData* metaData = metaDataCreatorFunc();
			if( !metaData )
				throw new Error( "The user-supplied meta-data creation function failed." );

			node->metaData = metaData;
			metaData->PostCreate( node );
		}

		// Choose a default name for the node as simply its type.
		node->SetName( type );

		// Kuplah, Worf!
		success = true;
	}
	catch( Error* error )
	{
		IssueError( error );
	}

	if( !success )
	{
		delete node;
		node = 0;
	}

	return node;
}

//=================================================================================
void Context::ClearAllErrors( void )
{
	for( std::list< Error* >::iterator iter = errorList->begin(); iter != errorList->end(); iter++ )
	{
		Error* error = *iter;
		delete error;
	}

	errorList->clear();
}

//=================================================================================
void Context::DumpAllErrors( FILE* fileStream )
{
	int index = 1;
	for( std::list< Error* >::iterator iter = errorList->begin(); iter != errorList->end(); iter++ )
	{
		Error* error = *iter;
		fprintf( fileStream, "%02d: %s\n", index++, error->ErrorMessage().c_str() );
	}
}

//=================================================================================
bool Context::IssueError( const char* errorMessageFormat, ... )
{
	va_list args;
	va_start( args, errorMessageFormat );
	Error* error = new Error( errorMessageFormat, args );
	errorList->push_back( error );
	va_end( args );

	// Return false here as a convenience to the caller who wants to return false for the error.
	return false;
}

//=================================================================================
bool Context::IssueError( Error* error )
{
	errorList->push_back( error );

	// Return false here as a convenience to the caller who wants to return false for the error.
	return false;
}

//=================================================================================
void Context::GetErrorStringList( std::list< std::string >& errorStringList )
{
	errorStringList.clear();
	for( std::list< Error* >::iterator iter = errorList->begin(); iter != errorList->end(); iter++ )
	{
		Error* error = *iter;
		errorStringList.push_back( error->ErrorMessage() );
	}
}

//=================================================================================
void Context::SetUserData( void* userData )
{
	this->userData = userData;
}

//=================================================================================
void* Context::GetUserData( void )
{
	return userData;
}

//=================================================================================
void Context::SetNodeReadingDisposition( NodeReadingDisposition nodeReadingDisposition )
{
	this->nodeReadingDisposition = nodeReadingDisposition;
}

//=================================================================================
Context::NodeReadingDisposition Context::GetNodeReadingDisposition( void )
{
	return nodeReadingDisposition;
}

//=================================================================================
void Context::SetAutoUpdateOverride( AutoUpdateOverride autoUpdateOverride )
{
	this->autoUpdateOverride = autoUpdateOverride;
}

//=================================================================================
Context::AutoUpdateOverride Context::GetAutoUpdateOverride( void )
{
	return autoUpdateOverride;
}

//=================================================================================
// Note that we don't provide any mechanism for invalidating cache entries
// when the file on disk is modified in a way that it doesn't match what
// we have loaded in cache memory.
Node* Context::TreeCacheLookup( const Path& path )
{
	if( !treeCacheMap )
		return 0;

	std::string key;
	if( !path.GetPath( key, true ) )
		return 0;

	TreeCacheMap::iterator iter = treeCacheMap->find( key );
	if( iter == treeCacheMap->end() )
		return 0;

	Node* root = iter->second;
	Node::CopyParameters copyParameters;
	Node* clone = root->Clone( *this, copyParameters );
	return clone;
}

//=================================================================================
// I wonder if we should overwrite the cache entry in the case that
// a cache entry for the given path already exists.
bool Context::TreeCacheAdd( const Path& path, const Node* root )
{
	if( !treeCacheMap )
		treeCacheMap = new TreeCacheMap();

	std::string key;
	if( !path.GetPath( key, true ) )
		return false;

	TreeCacheMap::iterator iter = treeCacheMap->find( key );
	if( iter != treeCacheMap->end() )
		return false;

	Node::CopyParameters copyParameters;
	Node* clone = root->Clone( *this, copyParameters );
	if( !clone )
		return false;

	( *treeCacheMap )[ key ] = clone;
	return true;
}

//=================================================================================
void Context::WipeTreeCache( void )
{
	if( treeCacheMap )
	{
		for( TreeCacheMap::iterator iter = treeCacheMap->begin(); iter != treeCacheMap->end(); iter++ )
		{
			Node* root = iter->second;
			delete root;
		}

		delete treeCacheMap;
		treeCacheMap = 0;
	}
}

// Context.cpp