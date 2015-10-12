// Context.h

namespace Cornucopia
{
	class Node;

	//=================================================================================
	class CORNUCOPIA_API Context
	{
	public:

		Context( void );
		virtual ~Context( void );

		typedef Node* ( *NodeCreatorFunc )( void );
		typedef MetaData* ( *MetaDataCreatorFunc )( void );

		// Registration and creation:
		bool RegisterNodeCreator( const std::string& type, NodeCreatorFunc nodeCreatorFunc );
		NodeCreatorFunc UnregisterNodeCreator( const std::string& type );
		NodeCreatorFunc LookupNodeCreator( const std::string& type );
		bool RegisterAllBuiltinNodeCreators( void );
		void SetMetaDataCreatorFunc( MetaDataCreatorFunc metaDataCreatorFunc );
		MetaDataCreatorFunc GetMetaDataCreatorFunc( void );
		void MakeListOfAllRegisteredNodeTypes( std::list< std::string >& nodeTypeList, bool sort = true );
		Node* CreateNode( const std::string& type );

		// Error handling:
		void ClearAllErrors( void );
		void DumpAllErrors( FILE* fileStream );
		bool IssueError( const char* errorMessageFormat, ... );
		bool IssueError( Error* error );
		void GetErrorStringList( std::list< std::string >& errorStringList );

		// The context is passed around the Cornucopia API and has periodic entry
		// points into user code, so provide a user data option.
		void SetUserData( void* userData );
		void* GetUserData( void );

		enum NodeReadingDisposition
		{
			NONEXISTENCE_OF_FIELD_IS_FATAL_ERROR,
			NONEXISTENCE_OF_FIELD_IS_DEFAULTED,
		};

		void SetNodeReadingDisposition( NodeReadingDisposition nodeReadingDisposition );
		NodeReadingDisposition GetNodeReadingDisposition( void );

		enum AutoUpdateOverride
		{
			DONT_OVERRIDE_AUTO_UPDATE,
			OVERRIDE_AUTO_UPDATE_AS_ALWAYS,
			OVERRIDE_AUTO_UPDATE_AS_NEVER,
		};

		void SetAutoUpdateOverride( AutoUpdateOverride autoUpdateOverride );
		AutoUpdateOverride GetAutoUpdateOverride( void );

		// Tree cache:
		Node* TreeCacheLookup( const Path& path );					// Return a clone of the cached tree, if any.
		bool TreeCacheAdd( const Path& path, const Node* root );	// Cache a clone of the given tree, if one doesn't already exist.
		void WipeTreeCache( void );

	private:

		typedef std::map< std::string, NodeCreatorFunc > CreationRegistryMap;
		typedef std::map< std::string, Node* > TreeCacheMap;

		std::list< Error* >* errorList;
		CreationRegistryMap* creationRegistry;
		MetaDataCreatorFunc metaDataCreatorFunc;
		void* userData;
		NodeReadingDisposition nodeReadingDisposition;
		AutoUpdateOverride autoUpdateOverride;
		TreeCacheMap* treeCacheMap;
	};
}

// Context.h