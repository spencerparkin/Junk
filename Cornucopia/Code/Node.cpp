// Node.cpp

#include "Cornucopia.h"

using namespace Cornucopia;

IMPL_CORNUCOPIA_CLASSINFO0( Node );

//=================================================================================
Node::Node( void )
{
	parent = 0;
	children = 0;
	metaData = 0;
	index = 0;
}

//=================================================================================
/*virtual*/ Node::~Node( void )
{
	DeleteChildren();

	if( metaData )
	{
		// This really isn't the best place to call
		// the pre-destroy method, though, because
		// all derived classes have already destructed.
		metaData->PreDestroy( this );

		delete metaData;
	}

	if( index )
		delete index;
}

//=================================================================================
void Node::DeleteChildren( void )
{
	if( children )
	{
		for( std::list< Node* >::iterator iter = children->begin(); iter != children->end(); iter++ )
		{
			Node* child = *iter;
			delete child;
		}

		delete children;
		children = 0;
	}
}

//=================================================================================
/*static*/ Node* Node::CreateAndReadFromFile( const std::string& file, Context& context, std::string* buffer /*= 0*/, int treeCacheFlags /*= 0*/ )
{
	Node* node = 0;
	lua_State* L = 0;

	context.ClearAllErrors();

	try
	{
		// If configured to check the tree cache first, check it.
		if( treeCacheFlags & TREE_CACHE_USE )
			node = context.TreeCacheLookup( file );
		
		// If we missed the tree cache or weren't configured to check it, go to disk.
		if( !node )
		{
			// Create a new Lua state to hold the contents of the file.
			L = lua_newstate( &Allocate, 0 );
			if( !L )
				throw new Error( "Failed to create Lua state in preparation for reading from file \"%s\".", file.c_str() );

			// Load the file onto the stack as an executable function.
			Reader reader( file, buffer );
			int luaError = lua_load( L, &Reader::Read, &reader, "cornucopia_chunk", "bt" );
			if( LUA_OK != luaError )
			{
				const char* luaError = lua_tostring( L, -1 );
				throw new Error( "Failed to load chunk from Lua file \"%s\", because \"%s\".", file.c_str(), luaError );
			}

			// Lua will load a function that return nil in the case that we really just couldn't open the file.
			DWORD errorCode = reader.GetErrorCode();
			if( errorCode == ERROR_FILE_NOT_FOUND || errorCode == ERROR_PATH_NOT_FOUND )
				throw new Error( "The file \"%s\" could not be found.", file.c_str() );
			if( errorCode != ERROR_SUCCESS )
				throw new Error( "Failed to open file \"%s\" with error code %d.", file.c_str(), errorCode );

			// Call the function.  It should return a table on the stack containing the entire node tree.
			if( LUA_OK != lua_pcall( L, 0, 1, 0 ) )
				throw new Error( "Failed to execute chunk loaded from Lua file \"%s\".", file.c_str() );

			// Did we get what we expected?
			if( lua_isnil( L, -1 ) || !lua_istable( L, -1 ) )
				throw new Error( "The Lua value returned by the chunk executed from the Lua file \"%s\" is not a table.", file.c_str() );

			// Yes.  Go create the node tree from the returned table.
			node = CreateAndReadFromTable( L, context );
			if( !node )
				throw new Error( "Failed to create root node from table." );
		}

		// At this point the tree chould be loaded into memory.
		// Perform an upgrade on the tree if possible and if configured to do so.
		if( !AutoUpgradeIfNecessary( file, node, context ) )
		{
			delete node;
			node = 0;
			throw new Error( "Failed to upgrade tree \"%s\".", file.c_str() );
		}

		// Lastly, if configured to add to tree cache, do so if it's not already added.
		if( treeCacheFlags & TREE_CACHE_ADD )
			context.TreeCacheAdd( file, node );
	}
	catch( Error* error )
	{
		context.IssueError( error );
	}

	// Be sure to clean up our mess before returning.
	if( L )
		lua_close( L );

	// Enjoy your meal.
	return node;
}

//=================================================================================
/*static*/ bool Node::AutoUpgradeIfNecessary( const std::string& file, Node*& node, Context& context )
{
	// The ability to up-grade a tree depends upon whether it has an origin reference node as its root.
	OriginReferenceNode* originRefNode = node->Cast< OriginReferenceNode >();
	if( originRefNode )
	{
		// In a production environment, it might be best to not use the auto-upgrade feature,
		// and instead, manually upgrade all content when a template tree changes.  But, this
		// requires all content authors to relinquish source control, which brings up the original
		// motivation behind auto-upgrade, which is that of preventing such a work-flow impediment.
		// Upgrading the template tree without impeding content authoring can be done with auto-upgrade,
		// and tools that load and process trees can also be assured that they load up-to-date versions.
		// A problem with this, however, is the increased overhead incurred in processing a bunch of content files.
		// So on one hand, auto-upgrade doesn't interrupt content author work-flow, but on the other
		// hand, it can create increased over-head in content file processing.  In a production environment,
		// I would probably rest a little more assured if auto-upgrade was off, and I could always find
		// a window of time to ugprade, when necessary, when all content authors are at home.
		bool autoUpgrade = originRefNode->GetAutoUpgrade();
		if( context.GetAutoUpdateOverride() == Context::OVERRIDE_AUTO_UPDATE_AS_ALWAYS )
			autoUpgrade = true;
		else if( context.GetAutoUpdateOverride() == Context::OVERRIDE_AUTO_UPDATE_AS_NEVER )
			autoUpgrade = false;
		if( autoUpgrade )
		{
			// If we're not the template tree, we can perform an upgrade.
			// The template tree references itself as origin, and of course, it
			// makes no sense to try to upgrade the template tree with itself.
			Path filePath( file );
			Path originFilePath;
			originRefNode->GetFilePath( originFilePath );
			if( !( originFilePath == filePath ) )
			{
				// It's a bit dumb that we have to create the entire tree to do the version check.
				// Alternatively, we could peak at the version in the file without doing any loading,
				// but that adds complexity that I'm not going to add now unless there becomes a real
				// need to do it from a performance stand-point.
				const Node* templateTree = node->CreateTemplateTree( context );
				if( !templateTree )
					return false;
				bool success = UpgradeTree( templateTree, node, context );
				delete templateTree;
				return success;
			}
		}
	}

	return true;
}

//=================================================================================
/*static*/ Node* Node::CreateAndReadFromTable( lua_State* L, Context& context )
{
	bool success = false;
	Node* node = 0;
	int top = lua_gettop( L );

	try
	{
		// What is the type of node represented by the given table?
		lua_getfield( L, -1, "type" );
		if( lua_isnil( L, -1 ) || !lua_isstring( L, -1 ) )
			throw new Error( "The node type of the given node table does not exist or is not a string." );
		std::string type = lua_tostring( L, -1 );
		lua_pop( L, 1 );

		// Try to create the node type.
		node = context.CreateNode( type );
		if( !node )
			throw new Error( "Failed to create node of type \"%s\".", type.c_str() );

		// Lastly, try to populate the node using the table contents.
		if( !node->ReadFromTable( L, context ) )
			throw new Error( "A node of type \"%s\", or one of its children, failed to read its node contents from a Lua table.", type.c_str() );

		// We made it!
		success = true;
	}
	catch( Error* error )
	{
		context.IssueError( error );
	}

	lua_settop( L, top );

	if( !success )
	{
		// Make sure we return nothing on failure.
		if( node )
		{
			delete node;
			node = 0;
		}
	}

	return node;
}

//=================================================================================
bool Node::WriteToFile( const std::string& file, Context& context, std::string* buffer /*= 0*/ ) const
{
	bool success = false;
	lua_State* L = 0;
	
	context.ClearAllErrors();

	try
	{
		// Create a new Lua state to hold the contents of the file.
		L = lua_newstate( &Allocate, 0 );
		if( !L )
			throw new Error( "Failed to create Lua state in preparation for writing to file \"%s\".", file.c_str() );

		// Create the table that represents this node.
		lua_newtable( L );
		if( !WriteToTable( L, context ) )
			throw new Error( "The node \"%s\" of type \"%s\", or one of its children, failed to write its node contents to a Lua table.", name.c_str(), GetType().c_str() );

		// Lastly, write the table out to disk.
		Writer writer( file, buffer );
		if( !writer.Write( L, context ) )
			throw new Error( "Failed to write Lua file \"%s\" for root node \"%s\" of type \"%s\".", file.c_str(), name.c_str(), GetType().c_str() );

		// Hurrah!
		success = true;
	}
	catch( Error* error )
	{
		context.IssueError( error );
	}

	// Be sure to clean up our mess before returning.
	if( L )
		lua_close( L );

	return success;
}

//=================================================================================
/*virtual*/ bool Node::ReadFromTable( lua_State* L, Context& context )
{
	bool success = false;
	int top = lua_gettop( L );

	try
	{
		// Read the node name.
		if( !ReadString( L, context, "name", name, "Unknown" ) )
			throw new Error( "Failed to read node name, because it does not exist or is not a string." );
		
		// Read the node explanations.
		for( int index = 0; index < EXPLANATION_COUNT; index++ )
		{
			explanation[ index ] = "";
			char explanationKey[ 128 ];
			sprintf_s( explanationKey, sizeof( explanationKey ), "explanation%d", index );
			if( !ReadString( L, context, explanationKey, explanation[ index ], "" ) )
				throw new Error( "Failed to read node \"%s\", because it does not exist or is not a string.", explanationKey );
		}

		// Are there any children?
		lua_getfield( L, -1, "children" );
		if( !lua_isnil( L, -1 ) )
		{
			// Yes.  Make sure it's a table.
			if( !lua_istable( L, -1 ) )
				throw new Error( "The children field of the given node table is not itself a table." );

			// Blow away any existing children.
			// We're going to recreate the list.
			DeleteChildren();
			children = new List();

			// Go read in all the child nodes.
			int index = 1;
			bool isnil = false;
			do
			{
				// Try to index the next child node.
				lua_pushinteger( L, index );
				lua_gettable( L, -2 );
				isnil = lua_isnil( L, -1 );
				if( !isnil )
				{
					// Make sure it's a table.
					if( !lua_istable( L, -1 ) )
						throw new Error( "The Lua value at index %d of the children table for node \"%s\" of type \"%s\" is not a table.", index, name.c_str(), GetType().c_str() );

					// Try to create the node from the table.
					Node* node = CreateAndReadFromTable( L, context );
					if( !node )
						throw new Error( "Failed to create child node %d of node \"%s\" of type \"%s\" and read its contents from a table.", index, name.c_str(), GetType().c_str() );

					// Build the tree.
					children->push_back( node );
					node->parent = this;
				}

				// Pop nil or the table and go to the next index.
				lua_pop( L, 1 );
				index++;
			}
			while( !isnil );
		}

		// Remove the children table or nil.
		lua_pop( L, 1 );

		// If we're capable of reading meta-data, now's the time to go read it.
		if( metaData )
		{
			lua_getfield( L, -1, "meta_data" );
			if( !lua_isnil( L, -1 ) && !metaData->ReadFromTable( L, context ) )
				throw new Error( "Node \"%s\" of type \"%s\" failed to read its meta-data.", name.c_str(), GetType().c_str() );
			lua_pop( L, 1 );
		}

		// We made it through the gauntlet!
		success = true;
	}
	catch( Error* error )
	{
		context.IssueError( error );
	}

	// Make sure we restore the stack to its original state.
	lua_settop( L, top );

	return success;
}

//=================================================================================
/*virtual*/ bool Node::WriteToTable( lua_State* L, Context& context ) const
{
	bool success = false;
	int top = lua_gettop( L );

	try
	{
		// Write the node type.
		lua_pushstring( L, GetType().c_str() );
		lua_setfield( L, -2, "type" );

		// Write the node name.
		lua_pushstring( L, name.c_str() );
		lua_setfield( L, -2, "name" );

		// Write the node explanations.
		for( int index = 0; index < EXPLANATION_COUNT; index++ )
		{
			char explanationKey[ 128 ];
			sprintf_s( explanationKey, sizeof( explanationKey ), "explanation%d", index );
			lua_pushstring( L, explanation[ index ].c_str() );
			lua_setfield( L, -2, explanationKey );
		}

		// Are there any children?
		if( children && children->size() > 0 )
		{
			// Yes.  Create the children table.
			lua_newtable( L );

			// Go populate the children table.
			int index = 1;
			for( List::iterator iter = children->begin(); iter != children->end(); iter++ )
			{
				Node* node = *iter;
				lua_pushinteger( L, index++ );
				lua_newtable( L );
				if( !node->WriteToTable( L, context ) )
					throw new Error( "The node \"%s\" of type \"%s\", or one of its children, failed to write its node contents to a Lua table.", node->name.c_str(), node->GetType().c_str() );
				lua_settable( L, -3 );
			}

			// Now that the table is populated, add it to the node table.
			// This also pops the table from the stack.
			lua_setfield( L, -2, "children" );
		}

		// If this node has meta-data, write that out too.
		if( metaData )
		{
			lua_newtable( L );
			if( !metaData->WriteToTable( L, context ) )
				throw new Error( "The node \"%s\" of type\"%s\" failed to write out its meta-data.", name.c_str(), GetType().c_str() );
			lua_setfield( L, -2, "meta_data" );
		}

		// We made it through the gauntlet!
		success = true;
	}
	catch( Error* error )
	{
		context.IssueError( error );
	}

	lua_settop( L, top );

	return success;
}

//=================================================================================
bool Node::DetachFromParent( Context& context, List::const_iterator* insertedBeforeIter /*= 0*/ )
{
	context.ClearAllErrors();

	if( !parent )
		return context.IssueError( "Cannot detach root node from its parent, because a root node doesn't have a parent." );

	if( !parent->children )
		return context.IssueError( "The tree is somehow corrupt, because a child's parent has no record of any of its children." );

	// This unfortunately changes the time complexity of detachment from O(1) to O(n) in the depth of the tree.
	parent->InvalidateIndices();

	// Remember where the child was located if the caller wants that.
	if( insertedBeforeIter && ChildLocation( *insertedBeforeIter ) )
		( *insertedBeforeIter )++;

	// Perform the removal.
	parent->children->remove( this );
	parent = 0;

	return true;
}

//=================================================================================
bool Node::AttachToParent( Node* parent, Context& context, List::const_iterator* insertBeforeIter /*= 0*/ )
{
	context.ClearAllErrors();

	if( this->parent )
		return context.IssueError( "Cannot attach node to parent, because the given node already has a parent." );

	if( this == parent || IsDescendant( parent ) )
		return context.IssueError( "Trees cannot have cycles.  An attempt to create a cyclical relationship has been detected and prevented." );

	if( !parent->children )
		parent->children = new List();

	if( insertBeforeIter && *insertBeforeIter != parent->children->end() )
	{
		const Node* beforeNode = **insertBeforeIter;
		if( beforeNode->parent != parent )
			return context.IssueError( "Cannot insert given node before given iterator, because these do not share the same parent." );

		parent->children->insert( *insertBeforeIter, this );
	}
	else
		parent->children->push_back( this );

	this->parent = parent;

	// This unfortunately changes the time complexity of attachment from O(1) to O(n) in the depth of the tree.
	parent->InvalidateIndices();

	return true;
}

//=================================================================================
// This is slow, but I don't expect a Cornucopia tree to ever be so big
// that an algorithm operating on such a tree will find this as a major
// bottle neck.
// Question: Does the uninitialized iterator always point to the end-of-list sentinal?
bool Node::ChildLocation( List::const_iterator& locationIter ) const
{
	if( !( parent && parent->children ) )
		return false;
	
	// We could probably get rid of this linear search if we cached an iterator with each node.
	for( locationIter = parent->children->begin(); locationIter != parent->children->end(); locationIter++ )
		if( *locationIter == this )
			break;
	
	if( locationIter == parent->children->end() )
		return false;

	return true;
}

//=================================================================================
bool Node::IsAncestor( const Node* node ) const
{
	Node* ancestor = parent;
	while( ancestor )
	{
		if( node == ancestor )
			return true;

		ancestor = ancestor->parent;
	}
	return false;
}

//=================================================================================
bool Node::IsDescendant( const Node* node ) const
{
	return node->IsAncestor( this );
}

//=================================================================================
// Simply get a sub-tree into a flat list.  We assume here that the given
// list is already empty, or that the user wants us to append to it.
void Node::MakeNodeList( List& list )
{
	list.push_back( this );

	if( children )
	{
		for( List::iterator iter = children->begin(); iter != children->end(); iter++ )
		{
			Node* child = *iter;
			child->MakeNodeList( list );
		}
	}
}

//=================================================================================
Node::CopyParameters::CopyParameters( CopyChildrenDisposition copyChildrenDisposition /*= COPY_NODE_AND_CHILDREN*/, bool copyMetaData /*= true*/, bool copyIdentityInfo /*= true*/ )
{
	this->copyChildrenDisposition = copyChildrenDisposition;
	this->copyMetaData = copyMetaData;
	this->copyIdentityInfo = copyIdentityInfo;
}

//=================================================================================
Node* Node::Clone( Context& context, const CopyParameters& copyParameters ) const
{
	bool success = false;
	Node* node = 0;

	context.ClearAllErrors();

	try
	{
		// Grab the registered creator for this node's type.
		Context::NodeCreatorFunc nodeCreatorFunc = context.LookupNodeCreator( GetType() );
		if( !nodeCreatorFunc )
			throw new Error( "Failed to clone node \"%s\" of type \"%s\", because its creator function is not found in the creation registry.", name.c_str(), GetType().c_str() );

		// Create a node of our type.
		node = ( *nodeCreatorFunc )();
		if( !node )
			throw new Error( "Failed to clone node \"%s\" of type \"%s\", because its creator function failed to allocate a new node of that type.", name.c_str(), GetType().c_str() );

		// Populate the newly created node with our contents.
		if( !node->Copy( this, context, copyParameters ) )
			throw new Error( "Failed to clone node \"%s\" of type \"%s\", because a failure occured while trying to copy the source node's contents into the destination node.", name.c_str(), GetType().c_str() );

		success = true;
	}
	catch( Error* error )
	{
		context.IssueError( error );
	}

	if( !success )
	{
		// Don't return a partial result on failure.
		if( node )
		{
			delete node;
			node = 0;
		}
	}

	return node;
}

//=================================================================================
/*virtual*/ bool Node::Copy( const Node* node, Context& context, const CopyParameters& copyParameters )
{
	if( copyParameters.copyIdentityInfo )
	{
		// Copy the name.
		name = node->name;

		// Copy the explanations.  In the idea of clonnig a template tree,
		// this explanation text will become very redundant and in that way,
		// potentially take up a lot of space unecessarily.
		for( int index = 0; index < EXPLANATION_COUNT; index++ )
			explanation[ index ] = node->explanation[ index ];
	}

	// We don't know who are parent is yet unless we're just copying the node value.
	if( copyParameters.copyChildrenDisposition != COPY_NODE_AND_LEAVE_CHILDREN_UNTOUCHED )
		parent = 0;
	
	// Remove children in preparation for copying over a new set of children, or do so if told to do so.
	if( copyParameters.copyChildrenDisposition == COPY_NODE_AND_CHILDREN || copyParameters.copyChildrenDisposition == COPY_NODE_AND_REMOVE_ALL_CHILDREN )
		DeleteChildren();

	// Clone the children of the given node if told to do so.
	if( node->children && copyParameters.copyChildrenDisposition == COPY_NODE_AND_CHILDREN )
	{
		children = new std::list< Node* >();

		for( List::iterator iter = node->children->begin(); iter != node->children->end(); iter++ )
		{
			Node* child = *iter;
			Node* clone = child->Clone( context, copyParameters );
			if( !clone )
				return context.IssueError( "Failed to clone node \"%s\" of type \"%s\".", child->name.c_str(), child->GetType().c_str() );

			children->push_back( clone );
			clone->parent = this;
		}
	}

	// Clone any meta-data.
	if( node->metaData && copyParameters.copyMetaData )
	{
		if( !metaData )
		{
			Context::MetaDataCreatorFunc metaDataCreatorFunc = context.GetMetaDataCreatorFunc();
			if( metaDataCreatorFunc )
				metaData = metaDataCreatorFunc();
			else
				return context.IssueError( "Failed to copy meta-data of node \"%s\" of type \"%s\", because there is no meta-data creator function.", name.c_str(), GetType().c_str() );
				
			if( !metaData )
				return context.IssueError( "Failed to copy meta-data of node \"%s\" of type \"%s\", because the meta-data creator failed.", name.c_str(), GetType().c_str() );

			metaData->PostCreate( this );
		}

		if( !metaData->Copy( node->metaData, context ) )
			return context.IssueError( "Failed to copy meta-data of node \"%s\" of type \"%s\".", name.c_str(), GetType().c_str() );
	}

	return true;
}

//=================================================================================
// This would typically be used to restore a default value from a template tree.
Node::CopyValueError Node::CopyValueFromTree( const Node* root, Context& context )
{
	List nodeList;
	if( !root->Find( nodeList, name, true ) )
		return CVE_NOT_FOUND;
	if( nodeList.size() > 1 )
	{
		context.IssueError( "A value cannot be copied from a tree where its associated name cannot be uniquely identified." );
		return CVE_NOT_UNIQUE;
	}

	const Node* node = *nodeList.begin();
	CopyParameters copyParameters( COPY_NODE_AND_LEAVE_CHILDREN_UNTOUCHED, false, false );
	if( !Copy( node, context, copyParameters ) )
		return CVE_COPY_ERROR;

	return CVE_NO_ERROR;
}

//=================================================================================
// This would typically be used during an upgrade of a tree.
Node::CopyValueError Node::CopyValueToTree( Node* root, Context& context ) const
{
	List nodeList;
	if( !root->Find( nodeList, name, true ) )
		return CVE_NOT_FOUND;
	if( nodeList.size() > 1 )
	{
		context.IssueError( "A value cannot be copied to a tree where its associated name cannot be uniquely identified." );
		return CVE_NOT_UNIQUE;
	}

	Node* node = *nodeList.begin();
	CopyParameters copyParameters( COPY_NODE_AND_LEAVE_CHILDREN_UNTOUCHED, false, false );
	if( !node->Copy( this, context, copyParameters ) )
		return CVE_COPY_ERROR;

	return CVE_NO_ERROR;
}

//=================================================================================
/*static*/ bool Node::UpgradeTree( const Node* templateTree, Node*& targetTree, Context& context )
{
	const OriginReferenceNode* templateOriginRefNode = templateTree->Cast< OriginReferenceNode >();
	if( !templateOriginRefNode )
		return context.IssueError( "The template tree has no origin node and therefore cannot serve as a template." );

	const OriginReferenceNode* clonedOriginRefNode = targetTree->Cast< OriginReferenceNode >();
	if( !clonedOriginRefNode )
		return context.IssueError( "The cloned tree has no origin node and therefore cannot be upgraded." );

	// Our job is done if the target tree is already up-to-date.
	// Proceeding anyway, however, shouldn't have changed anything.
	if( clonedOriginRefNode->GetVersion() == templateOriginRefNode->GetVersion() )
		return true;

	// Begin the upgraded tree as a clone of the template tree.
	CopyParameters copyParameters;
	Node* upgradedTree = templateTree->Clone( context, copyParameters );

	// Now throw all values in the target tree into the upgraded tree.
	List nodeList;
	targetTree->MakeNodeList( nodeList );
	List::iterator iter;
	for( iter = nodeList.begin(); iter != nodeList.end(); iter++ )
	{
		Node* node = *iter;

		// The only node we don't preserve is the origin node, which makes sense,
		// because the template tree always owns the most up-date version of this node.
		// Clones of templates are not supposed to edit the origin node.
		if( node->ThisClassInfo()->Type() != OriginReferenceNode::classInfo.Type() )
		{
			// A copy failure here in the case that the data is not found is
			// not an error, because the template may no longer support the
			// authored data.
			CopyValueError cve = node->CopyValueToTree( upgradedTree, context );

			// If the choice of where to copy the value is ambiguous, however,
			// then there is a problem with the new template.
			if( cve == CVE_NOT_UNIQUE || cve == CVE_COPY_ERROR )
				break;
		}
	}

	// Failure occurs if we can't uniquely identify nodes.
	if( iter != nodeList.end() )
	{
		delete upgradedTree;
		return context.IssueError( "A problem occured while attempting to upgrade the cloned tree.  It will be left untouched.  The template tree needs revisiting." );
	}
	
	// Now simply replace the target tree with the upgraded tree.
	delete targetTree;
	targetTree = upgradedTree;
	return true;
}

//=================================================================================
Node* Node::CreateTemplateTree( Context& context ) const
{
	const Node* root = const_cast< Node* >( this )->FindRoot();
	const OriginReferenceNode* originRefNode = root->Cast< OriginReferenceNode >();
	if( !originRefNode )
	{
		context.IssueError( "A template tree cannot be created from a tree that has no origin node." );
		return 0;
	}

	Path filePath;
	if( !originRefNode->GetFilePath( filePath ) )
		return 0;

	Node* templateTree = CreateAndReadFromFile( filePath, context, 0, TREE_CACHE_USE | TREE_CACHE_ADD );
	return templateTree;
}

//=================================================================================
bool Node::RestoreDefaultValue( Context& context, const Node* templateTree /*= 0*/ )
{
	bool deleteTemplate = false;

	if( !templateTree )
	{
		templateTree = CreateTemplateTree( context );
		if( !templateTree )
			return context.IssueError( "Value restoration is not possible without the template tree." );

		deleteTemplate = true;
	}

	bool success = false;
	if( CVE_NO_ERROR == CopyValueFromTree( templateTree, context ) )
		success = true;
	
	if( deleteTemplate )
		delete templateTree;

	return success;
}

//=================================================================================
// We assume that if there is an origin root node, then default values can be restored.
bool Node::DefaultValueCanBeRestored( void ) const
{
	Node* root = const_cast< Node* >( this )->FindRoot();
	Cornucopia::OriginReferenceNode* originRefNode = root->Cast< Cornucopia::OriginReferenceNode >();
	if( originRefNode )
		return true;
	return false;
}

//=================================================================================
Node* Node::FindRoot( void )
{
	Node* root = this;
	while( root->parent )
		root = root->parent;
	return root;
}

//=================================================================================
const std::string& Node::GetType( void ) const
{
	return ThisClassInfo()->Type();
}

//=================================================================================
const std::string& Node::GetName( void ) const
{
	return name;
}

//=================================================================================
void Node::SetName( const std::string& name )
{
	this->name = name;
}

//=================================================================================
const std::string& Node::GetExplanation( WhichExplanation whichExplanation ) const
{
	if( whichExplanation == EXPLANATION_COUNT )
		whichExplanation = SHORT_EXPLANATION;
	return explanation[ whichExplanation ];
}

//=================================================================================
void Node::SetExplanation( WhichExplanation whichExplanation, const std::string& explanation )
{
	if( whichExplanation != EXPLANATION_COUNT )
		this->explanation[ whichExplanation ] = explanation;
}

//=================================================================================
/*virtual*/ std::string Node::GetValueToDisplayString( void ) const
{
	std::string valueDisplayString;
	if( VSE_NONE == GetValueToString( valueDisplayString ) )
		return valueDisplayString;
	return "N/A";
}

//=================================================================================
/*virtual*/ Node::ValueStringError Node::GetValueToString( std::string& valueString ) const
{
	return VSE_NOT_SUPPORTED;
}

//=================================================================================
/*virtual*/ Node::ValueStringError Node::SetValueFromString( const std::string& valueString )
{
	return VSE_NOT_SUPPORTED;
}

//=================================================================================
/*virtual*/ Node::ValueStringError Node::GetValueAsStringHelp( std::string& helpString ) const
{
	return VSE_NOT_SUPPORTED;
}

//=================================================================================
const Node* Node::GetParent( void ) const
{
	return parent;
}

//=================================================================================
Node* Node::GetParent( void )
{
	return parent;
}

//=================================================================================
const Node::List* Node::GetChildren( void ) const
{
	return children;
}

//=================================================================================
Node::List* Node::GetChildren( void )
{
	return children;
}

//=================================================================================
MetaData* Node::GetMetaData( void )
{
	return metaData;
}

//=================================================================================
const MetaData* Node::GetMetaData( void ) const
{
	return metaData;
}

//=================================================================================
bool Node::SubTreeContainsNode( Node* node ) const
{
	if( this == node )
		return true;

	if( children )
	{
		for( List::iterator iter = children->begin(); iter != children->end(); iter++ )
		{
			Node* child = *iter;
			if( child->SubTreeContainsNode( node ) )
				return true;
		}
	}

	return false;
}

//=================================================================================
/*static*/ bool Node::ConvertValueStringToVarMap( const std::string& valueString, VarMap& varMap )
{
	enum Expecting
	{
		EXPECTING_KEY,
		EXPECTING_ASSIGNMENT,
		EXPECTING_NUMERICAL_VALUE,
		EXPECTING_COMMA,
	};

	boost::char_separator< char > separator( " ", ",=" );
	typedef boost::tokenizer< boost::char_separator< char > > Tokenizer;
	Tokenizer tokenizer( valueString, separator );
	Expecting expecting = EXPECTING_KEY;
	std::string key, value;
	bool keyValuePair = false;
	for( Tokenizer::iterator iter = tokenizer.begin(); iter != tokenizer.end(); iter++ )
	{
		std::string token = *iter;

		// Lexically analyze the parsed tokens.
		switch( expecting )
		{
			case EXPECTING_KEY:
			{
				if( !std::isalpha( token[0] ) )
					return false;
				key = token;
				expecting = EXPECTING_ASSIGNMENT;
				break;
			}
			case EXPECTING_ASSIGNMENT:
			{
				if( token != "=" )
					return false;
				expecting = EXPECTING_NUMERICAL_VALUE;
				break;
			}
			case EXPECTING_NUMERICAL_VALUE:
			{
				if( !( std::isdigit( token[0] ) || token[0] == '-' ) )
					return false;
				value = token;
				expecting = EXPECTING_COMMA;
				keyValuePair = true;
				break;
			}
			case EXPECTING_COMMA:
			{
				if( token != "," )
					return false;
				expecting = EXPECTING_KEY;
				break;
			}
		}

		if( keyValuePair )
		{
			double numericalValue = 0.0;
			std::istringstream stream( value );
			stream >> numericalValue;
			varMap[ key ] = numericalValue;
			keyValuePair = false;
		}
	}

	if( expecting != EXPECTING_COMMA && expecting != EXPECTING_KEY )
		return false;

	return true;
}

//=================================================================================
/*static*/ bool Node::ConvertValueStringFromVarMap( std::string& valueString, const VarMap& varMap )
{
	valueString = "";
	for( VarMap::const_iterator iter = varMap.begin(); iter != varMap.end(); iter++ )
	{
		std::string key = iter->first;
		double numericalValue = iter->second;
		std::ostringstream stream;
		stream << numericalValue;
		std::string value = stream.str();
		if( !valueString.empty() )
			valueString += ", ";
		valueString += key + "=" + value;
	}

	return true;
}

//=================================================================================
/*static*/ bool Node::ReadVectorE2GA( lua_State* L, Context& context, const std::string& name, c3ga::vectorE2GA& vector, const c3ga::vectorE2GA* defaultValue /*= 0*/ )
{
	bool success = false;
	int top = lua_gettop( L );

	try
	{
		if( defaultValue )
			vector = *defaultValue;
		else
		{
			vector.set_e1( 0.0 );
			vector.set_e2( 0.0 );
		}

		lua_getfield( L, -1, name.c_str() );
		if( lua_isnil( L, -1 ) || !lua_istable( L, -1 ) )
		{
			if( context.GetNodeReadingDisposition() == Context::NONEXISTENCE_OF_FIELD_IS_FATAL_ERROR )
				throw new Error( "ReadVectorE2GA failed to read \"%s\" as a table.", name.c_str() );
		}
		else
		{
			if( !ReadNumber( L, context, "x", vector.m_e1, 0.0 ) )
				throw new Error( "ReadVectorE2GA failed to read \"x\" from \"%s\".", name.c_str() );

			if( !ReadNumber( L, context, "y", vector.m_e2, 0.0 ) )
				throw new Error( "ReadVectorE2GA failed to read \"y\" from \"%s\".", name.c_str() );
		}

		// Pop the vector table.
		lua_pop( L, 1 );

		success = true;
	}
	catch( Error* error )
	{
		context.IssueError( error );
	}

	lua_settop( L, top );

	return success;
}

//=================================================================================
/*static*/ bool Node::WriteVectorE2GA( lua_State* L, Context& context, const std::string& name, const c3ga::vectorE2GA& vector )
{
	lua_newtable( L );

	WriteNumber( L, context, "x", vector.get_e1() );
	WriteNumber( L, context, "y", vector.get_e2() );

	lua_setfield( L, -2, name.c_str() );

	return true;
}

//=================================================================================
/*static*/ bool Node::ReadBivectorE2GA( lua_State* L, Context& context, const std::string& name, c3ga::bivectorE2GA& bivector, const c3ga::bivectorE2GA* defaultValue /*= 0*/ )
{
	bool success = false;
	int top = lua_gettop( L );

	try
	{
		if( defaultValue )
			bivector = *defaultValue;
		else
			bivector.set_e1_e2( 0.0 );

		lua_getfield( L, -1, name.c_str() );
		if( lua_isnil( L, -1 ) || !lua_istable( L, -1 ) )
		{
			if( context.GetNodeReadingDisposition() == Context::NONEXISTENCE_OF_FIELD_IS_FATAL_ERROR )
				throw new Error( "ReadBivectorE2GA failed to read \"%s\" as a table.", name.c_str() );
		}
		else
		{
			if( !ReadNumber( L, context, "xy", bivector.m_e1_e2, 0.0 ) )
				throw new Error( "ReadBivectorE2GA failed to read \"xy\" from \"%s\".", name.c_str() );
		}

		// Pop the bivector table.
		lua_pop( L, 1 );

		success = true;
	}
	catch( Error* error )
	{
		context.IssueError( error );
	}

	lua_settop( L, top );

	return success;
}

//=================================================================================
/*static*/ bool Node::WriteBivectorE2GA( lua_State* L, Context& context, const std::string& name, const c3ga::bivectorE2GA& bivector )
{
	lua_newtable( L );

	WriteNumber( L, context, "xy", bivector.get_e1_e2() );

	lua_setfield( L, -2, name.c_str() );

	return true;
}

//=================================================================================
/*static*/ bool Node::ReadVectorE3GA( lua_State* L, Context& context, const std::string& name, c3ga::vectorE3GA& vector, const c3ga::vectorE3GA* defaultValue /*= 0*/ )
{
	bool success = false;
	int top = lua_gettop( L );

	try
	{
		if( defaultValue )
			vector = *defaultValue;
		else
			vector.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 0.0 );

		lua_getfield( L, -1, name.c_str() );
		if( lua_isnil( L, -1 ) || !lua_istable( L, -1 ) )
		{
			if( context.GetNodeReadingDisposition() == Context::NONEXISTENCE_OF_FIELD_IS_FATAL_ERROR )
				throw new Error( "ReadVectorE3GA failed to read \"%s\" as a table.", name.c_str() );
		}
		else
		{
			if( !ReadNumber( L, context, "x", vector.m_e1, 0.0 ) )
				throw new Error( "ReadVectorE3GA failed to read \"x\" from \"%s\".", name.c_str() );

			if( !ReadNumber( L, context, "y", vector.m_e2, 0.0 ) )
				throw new Error( "ReadVectorE3GA failed to read \"y\" from \"%s\".", name.c_str() );

			if( !ReadNumber( L, context, "z", vector.m_e3, 0.0 ) )
				throw new Error( "ReadVectorE3GA failed to read \"z\" from \"%s\".", name.c_str() );
		}

		// Pop the vector table.
		lua_pop( L, 1 );

		success = true;
	}
	catch( Error* error )
	{
		context.IssueError( error );
	}

	lua_settop( L, top );

	return success;
}

//=================================================================================
/*static*/ bool Node::WriteVectorE3GA( lua_State* L, Context& context, const std::string& name, const c3ga::vectorE3GA& vector )
{
	lua_newtable( L );

	WriteNumber( L, context, "x", vector.get_e1() );
	WriteNumber( L, context, "y", vector.get_e2() );
	WriteNumber( L, context, "z", vector.get_e3() );

	lua_setfield( L, -2, name.c_str() );

	return true;
}

//=================================================================================
/*static*/ bool Node::ReadBivectorE3GA( lua_State* L, Context& context, const std::string& name, c3ga::bivectorE3GA& bivector, const c3ga::bivectorE3GA* defaultValue /*= 0*/ )
{
	bool success = false;
	int top = lua_gettop( L );

	try
	{
		if( defaultValue )
			bivector = *defaultValue;
		else
			bivector.set( c3ga::bivectorE3GA::coord_e1e2_e2e3_e3e1, 0.0, 0.0, 0.0 );

		lua_getfield( L, -1, name.c_str() );
		if( lua_isnil( L, -1 ) || !lua_istable( L, -1 ) )
		{
			if( context.GetNodeReadingDisposition() == Context::NONEXISTENCE_OF_FIELD_IS_FATAL_ERROR )
				throw new Error( "ReadBivectorE3GA failed to read \"%s\" as a table.", name.c_str() );
		}
		else
		{
			if( !ReadNumber( L, context, "xy", bivector.m_e1_e2, 0.0 ) )
				throw new Error( "ReadBivectorE3GA failed to read \"xy\" from \"%s\".", name.c_str() );

			if( !ReadNumber( L, context, "yz", bivector.m_e2_e3, 0.0 ) )
				throw new Error( "ReadBivectorE3GA failed to read \"yz\" from \"%s\".", name.c_str() );

			if( !ReadNumber( L, context, "zx", bivector.m_e3_e1, 0.0 ) )
				throw new Error( "ReadBivectorE3GA failed to read \"zx\" from \"%s\".", name.c_str() );
		}

		// Pop the bivector table.
		lua_pop( L, 1 );

		success = true;
	}
	catch( Error* error )
	{
		context.IssueError( error );
	}

	lua_settop( L, top );

	return success;
}

//=================================================================================
/*static*/ bool Node::WriteBivectorE3GA( lua_State* L, Context& context, const std::string& name, const c3ga::bivectorE3GA& bivector )
{
	lua_newtable( L );

	WriteNumber( L, context, "xy", bivector.get_e1_e2() );
	WriteNumber( L, context, "yz", bivector.get_e2_e3() );
	WriteNumber( L, context, "zx", bivector.get_e3_e1() );

	lua_setfield( L, -2, name.c_str() );

	return true;
}

//=================================================================================
/*static*/ bool Node::ReadRotorE3GA( lua_State* L, Context& context, const std::string& name, c3ga::rotorE3GA& rotor, const c3ga::rotorE3GA* defaultValue /*= 0*/ )
{
	bool success = false;
	int top = lua_gettop( L );

	try
	{
		if( defaultValue )
			rotor = *defaultValue;
		else
			rotor.set( c3ga::rotorE3GA::coord_scalar_e1e2_e2e3_e3e1, 1.0, 0.0, 0.0, 0.0 );

		lua_getfield( L, -1, name.c_str() );
		if( lua_isnil( L, -1 ) || !lua_istable( L, -1 ) )
		{
			if( context.GetNodeReadingDisposition() == Context::NONEXISTENCE_OF_FIELD_IS_FATAL_ERROR )
				throw Error( "ReadRotorE3GA failed to read \"%s\" as a table.", name.c_str() );
		}
		else
		{
			if( !ReadNumber( L, context, "scalar", rotor.m_scalar, 1.0 ) )
				throw new Error( "ReadRotorE3GA failed to read \"scalar\" from \"%s\".", name.c_str() );

			if( !ReadNumber( L, context, "xy", rotor.m_e1_e2, 0.0 ) )
				throw new Error( "ReadRotorE3GA failed to read \"xy\" from \"%s\".", name.c_str() );

			if( !ReadNumber( L, context, "yz", rotor.m_e2_e3, 0.0 ) )
				throw new Error( "ReadRotorE3GA failed to read \"yz\" from \"%s\".", name.c_str() );

			if( !ReadNumber( L, context, "zx", rotor.m_e3_e1, 0.0 ) )
				throw new Error( "ReadRotorE3GA failed to read \"zx\" from \"%s\".", name.c_str() );
		}

		lua_pop( L, 1 );

		success = true;
	}
	catch( Error* error )
	{
		context.IssueError( error );
	}

	lua_settop( L, top );

	return success;
}

//=================================================================================
/*static*/ bool Node::WriteRotorE3GA( lua_State* L, Context& context, const std::string& name, const c3ga::rotorE3GA& rotor )
{
	lua_newtable( L );

	WriteNumber( L, context, "scalar", rotor.get_scalar() );
	WriteNumber( L, context, "xy", rotor.get_e1_e2() );
	WriteNumber( L, context, "yz", rotor.get_e2_e3() );
	WriteNumber( L, context, "zx", rotor.get_e3_e1() );

	lua_setfield( L, -2, name.c_str() );

	return true;
}

//=================================================================================
/*static*/ bool Node::ReadNumber( lua_State* L, Context& context, const std::string& name, double& value, double defaultValue )
{
	value = defaultValue;
	lua_getfield( L, -1, name.c_str() );
	if( !lua_isnil( L, -1 ) && lua_isnumber( L, -1 ) )
		value = lua_tonumber( L, -1 );
	else if( context.GetNodeReadingDisposition() == Context::NONEXISTENCE_OF_FIELD_IS_FATAL_ERROR )
	{
		lua_pop( L, 1 );
		return context.IssueError( "Failed to read number from field \"%s\".", name.c_str() );
	}
	lua_pop( L, 1 );
	return true;
}

//=================================================================================
/*static*/ bool Node::WriteNumber( lua_State* L, Context& context, const std::string& name, double value )
{
	lua_pushnumber( L, value );
	lua_setfield( L, -2, name.c_str() );
	return true;
}

//=================================================================================
/*static*/ bool Node::ReadInteger( lua_State* L, Context& context, const std::string& name, int& value, int defaultValue )
{
	value = defaultValue;
	lua_getfield( L, -1, name.c_str() );
	if( !lua_isnil( L, -1 ) && lua_isnumber( L, -1 ) )
		value = lua_tointeger( L, -1 );
	else if( context.GetNodeReadingDisposition() == Context::NONEXISTENCE_OF_FIELD_IS_FATAL_ERROR )
	{
		lua_pop( L, 1 );
		return context.IssueError( "Failed to read integer from field \"%s\".", name.c_str() );
	}
	lua_pop( L, 1 );
	return true;
}

//=================================================================================
/*static*/ bool Node::WriteInteger( lua_State* L, Context& context, const std::string& name, int value )
{
	lua_pushinteger( L, value );
	lua_setfield( L, -2, name.c_str() );
	return true;
}

//=================================================================================
/*static*/ bool Node::ReadString( lua_State* L, Context& context, const std::string& name, std::string& value, const std::string& defaultValue )
{
	value = defaultValue;
	lua_getfield( L, -1, name.c_str() );
	if( !lua_isnil( L, -1 ) && lua_isstring( L, -1 ) )
		value = lua_tostring( L, -1 );
	else if( context.GetNodeReadingDisposition() == Context::NONEXISTENCE_OF_FIELD_IS_FATAL_ERROR )
	{
		lua_pop( L, 1 );
		return context.IssueError( "Failed to read string from field \"%s\".", name.c_str() );
	}
	lua_pop( L, 1 );
	return true;
}

//=================================================================================
/*static*/ bool Node::WriteString( lua_State* L, Context& context, const std::string& name, const std::string& value )
{
	lua_pushstring( L, value.c_str() );
	lua_setfield( L, -2, name.c_str() );
	return true;
}

//=================================================================================
/*static*/ bool Node::ReadBoolean( lua_State* L, Context& context, const std::string& name, bool& value, bool defaultValue )
{
	value = defaultValue;
	lua_getfield( L, -1, name.c_str() );
	if( !lua_isnil( L, -1 ) && lua_isboolean( L, -1 ) )
		value = lua_toboolean( L, -1 ) ? true : false;
	else if( context.GetNodeReadingDisposition() == Context::NONEXISTENCE_OF_FIELD_IS_FATAL_ERROR )
	{
		lua_pop( L, 1 );
		return context.IssueError( "Failed to read boolean from field \"%s\".", name.c_str() );
	}
	lua_pop( L, 1 );
	return true;
}

//=================================================================================
/*static*/ bool Node::WriteBoolean( lua_State* L, Context& context, const std::string& name, bool value )
{
	lua_pushboolean( L, ( value ? 1 : 0 ) );
	lua_setfield( L, -2, name.c_str() );
	return true;
}

//=================================================================================
// Failure to invalidate indices where appropriate can lead to a stale pointer
// dereference bug!
void Node::InvalidateIndices( void ) const
{
	const Node* node = this;
	do
	{
		if( node->index )
			node->index->Invalidate();
		node = node->parent;
	}
	while( node );
}

//=================================================================================
bool Node::Find( List& nodeList, const std::string& name, bool useIndex /*= false*/ ) const
{
	if( useIndex )
	{
		// Try to find a valid index beginning at this node.
		Index** index = 0;
		const Node* node = this;
		do
		{
			index = &node->index;
			if( *index && ( *index )->IsValid() )
				break;
			node = node->parent;
		}
		while( node );

		// If no valid index was found, make one at this node.
		if( !*index || !( *index )->IsValid() )
		{
			index = &this->index;
			if( !*index )
				*index = new Index();
			( *index )->Rebuild( this );
		}

		// Now use the index to perform the search.
		return ( *index )->Lookup( nodeList, name );
	}
	
	// Go do a linear search.
	nodeList.clear();
	List subTreeList;
	const_cast< Node* >( this )->MakeNodeList( subTreeList );
	for( List::iterator iter = subTreeList.begin(); iter != subTreeList.end(); iter++ )
	{
		Node* node = *iter;
		if( node->GetName() == name )
			nodeList.push_back( node );
	}

	if( nodeList.size() > 0 )
		return true;
	return false;
}

//=================================================================================
Node::Index::Index( void )
{
	valid = false;
}

//=================================================================================
Node::Index::~Index( void )
{
	Delete();
}

//=================================================================================
// We return a list here, because Cornucopia does not enforce unique names in the tree.
bool Node::Index::Lookup( List& nodeList, const std::string& name )
{
	nodeList.clear();

	if( !valid )
		return false;

	std::map< std::string, List* >::iterator mapIter = nodeMap.find( name );
	if( mapIter == nodeMap.end() )
		return false;

	nodeList = *mapIter->second;
	return true;
}

//=================================================================================
bool Node::Index::IsValid( void )
{
	return valid;
}

//=================================================================================
void Node::Index::Invalidate( void )
{
	valid = false;
}

//=================================================================================
void Node::Index::Delete( void )
{
	for( std::map< std::string, List* >::iterator mapIter = nodeMap.begin(); mapIter != nodeMap.end(); mapIter++ )
	{
		List* nodeList = mapIter->second;
		delete nodeList;
	}
}

//=================================================================================
void Node::Index::Rebuild( const Node* root )
{
	Delete();

	List subTreeList;
	const_cast< Node* >( root )->MakeNodeList( subTreeList );

	for( List::const_iterator iter = subTreeList.begin(); iter != subTreeList.end(); iter++ )
	{
		Node* node = *iter;
		std::string name = node->GetName();
		
		List* nodeList = 0;
		std::map< std::string, List* >::iterator mapIter = nodeMap.find( name );
		if( mapIter == nodeMap.end() )
			nodeMap[ name ] = nodeList = new List();
		else
			nodeList = mapIter->second;

		nodeList->push_back( node );
	}

	valid = true;
}

// Node.cpp