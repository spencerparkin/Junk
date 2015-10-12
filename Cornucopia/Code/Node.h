// Node.h

namespace Cornucopia
{
	//=================================================================================
	class CORNUCOPIA_API Node
	{
		friend class Context;

		DECL_CORNUCOPIA_CLASSINFO( Node );

	public:

		typedef std::list< Node* > List;

		Node( void );
		virtual ~Node( void );

		enum
		{
			TREE_CACHE_ADD		= 0x00000001,		// Add to the tree cache if not already added.
			TREE_CACHE_USE		= 0x00000002,		// Hit the tree cache if already added.
		};

		static Node* CreateAndReadFromFile( const std::string& file, Context& context, std::string* buffer = 0, int treeCacheFlags = 0 );
		bool WriteToFile( const std::string& file, Context& context, std::string* buffer = 0 ) const;

		// Any override of these virtual methods should call the immediate base class implementation!
		// The caller can assume that a table exists at the top of the stack.  It is expected that the
		// caller will leave the stack untouched.  You can manipulate the stack, but at the end of the
		// call it must look as it was at the start of the call.
		virtual bool ReadFromTable( lua_State* L, Context& context );
		virtual bool WriteToTable( lua_State* L, Context& context ) const;

		void DeleteChildren( void );

		bool DetachFromParent( Context& context, List::const_iterator* insertedBeforeIter = 0 );
		bool AttachToParent( Node* parent, Context& context, List::const_iterator* insertBeforeIter = 0 );

		bool ChildLocation( List::const_iterator& locationIter ) const;

		enum CopyChildrenDisposition
		{
			COPY_NODE_AND_CHILDREN,
			COPY_NODE_AND_REMOVE_ALL_CHILDREN,
			COPY_NODE_AND_LEAVE_CHILDREN_UNTOUCHED,
		};

		class CORNUCOPIA_API CopyParameters
		{
		public:
			CopyParameters( CopyChildrenDisposition copyChildrenDisposition = COPY_NODE_AND_CHILDREN, bool copyMetaData = true, bool copyIdentityInfo = true );

			CopyChildrenDisposition copyChildrenDisposition;
			bool copyMetaData;
			bool copyIdentityInfo;
		};

		Node* Clone( Context& context, const CopyParameters& copyParameters ) const;

		// The implementation can always assume that the given node can always be safely cast to
		// the derived class type.  It should also always call the immediate base class implementation
		// to insure that a full copy is performed.
		virtual bool Copy( const Node* node, Context& context, const CopyParameters& copyParameters );

		const std::string& GetType( void ) const;
		const std::string& GetName( void ) const;
		void SetName( const std::string& name );

		enum WhichExplanation { SHORT_EXPLANATION, LONG_EXPLANATION, EXPLANATION_COUNT };
		const std::string& GetExplanation( WhichExplanation whichExplanation ) const;
		void SetExplanation( WhichExplanation whichExplanation, const std::string& explanation );
		
		// Every node should support this.  Just return a description
		// of the contents of your node for display purposes.
		virtual std::string GetValueToDisplayString( void ) const;

		enum ValueStringError
		{
			VSE_NONE,				// No error.  The value was accepted.
			VSE_NO_CHANGE,			// No error, but the value was not changed.
			VSE_SYNTAX,				// The value string had a syntax error.
			VSE_INVALID,			// The value string was syntacitcally correct, but had invalid data.
			VSE_NOT_SUPPORTED,		// Setting the node's value by string is not supported by the node type.
			VSE_UNKNOWN,			// Some unknown error occurred.
		};

		// These functions are meant to be inverses of one another.
		virtual ValueStringError GetValueToString( std::string& valueString ) const;
		virtual ValueStringError SetValueFromString( const std::string& valueString );

		// Provide a grammer or syntax that will explain how this node can be set using a string, if supported.
		virtual ValueStringError GetValueAsStringHelp( std::string& helpString ) const;

		const Node* GetParent( void ) const;
		Node* GetParent( void );
		const List* GetChildren( void ) const;
		List* GetChildren( void );

		// We assume that the given list is already empty.
		void MakeNodeList( List& list );

		bool IsAncestor( const Node* node ) const;
		bool IsDescendant( const Node* node ) const;

		MetaData* GetMetaData( void );
		const MetaData* GetMetaData( void ) const;

		bool SubTreeContainsNode( Node* node ) const;

		bool Find( List& nodeList, const std::string& name, bool useIndex = false ) const;
		void InvalidateIndices( void ) const;

		Node* FindRoot( void );

		enum CopyValueError
		{
			CVE_NO_ERROR,
			CVE_COPY_ERROR,
			CVE_NOT_FOUND,
			CVE_NOT_UNIQUE,
		};

		CopyValueError CopyValueFromTree( const Node* root, Context& context );
		CopyValueError CopyValueToTree( Node* root, Context& context ) const;

		// This operation won't work unles the given template and target trees have unique node names.
		static bool UpgradeTree( const Node* templateTree, Node*& targetTree, Context& context );

		bool RestoreDefaultValue( Context& context, const Node* templateTree = 0 );
		bool DefaultValueCanBeRestored( void ) const;

		Node* CreateTemplateTree( Context& context ) const;

	protected:

		typedef std::map< std::string, double > VarMap;
		static bool ConvertValueStringToVarMap( const std::string& valueString, VarMap& varMap );
		static bool ConvertValueStringFromVarMap( std::string& valueString, const VarMap& varMap );

		static bool ReadVectorE2GA( lua_State* L, Context& context, const std::string& name, c3ga::vectorE2GA& vector, const c3ga::vectorE2GA* defaultValue = 0 );
		static bool WriteVectorE2GA( lua_State* L, Context& context, const std::string& name, const c3ga::vectorE2GA& vector );

		static bool ReadBivectorE2GA( lua_State* L, Context& context, const std::string& name, c3ga::bivectorE2GA& bivector, const c3ga::bivectorE2GA* defaultValue = 0 );
		static bool WriteBivectorE2GA( lua_State* L, Context& context, const std::string& name, const c3ga::bivectorE2GA& bivector );

		static bool ReadVectorE3GA( lua_State* L, Context& context, const std::string& name, c3ga::vectorE3GA& vector, const c3ga::vectorE3GA* defaultValue = 0 );
		static bool WriteVectorE3GA( lua_State* L, Context& context, const std::string& name, const c3ga::vectorE3GA& vector );

		static bool ReadBivectorE3GA( lua_State* L, Context& context, const std::string& name, c3ga::bivectorE3GA& bivector, const c3ga::bivectorE3GA* defaultValue = 0 );
		static bool WriteBivectorE3GA( lua_State* L, Context& context, const std::string& name, const c3ga::bivectorE3GA& bivector );

		static bool ReadRotorE3GA( lua_State* L, Context& context, const std::string& name, c3ga::rotorE3GA& rotor, const c3ga::rotorE3GA* defaultValue = 0 );
		static bool WriteRotorE3GA( lua_State* L, Context& context, const std::string& name, const c3ga::rotorE3GA& rotor );

		static bool ReadNumber( lua_State* L, Context& context, const std::string& name, double& value, double defaultValue );
		static bool WriteNumber( lua_State* L, Context& context, const std::string& name, double value );

		static bool ReadInteger( lua_State* L, Context& context, const std::string& name, int& value, int defaultValue );
		static bool WriteInteger( lua_State* L, Context& context, const std::string& name, int value );

		static bool ReadString( lua_State* L, Context& context, const std::string& name, std::string& value, const std::string& defaultValue );
		static bool WriteString( lua_State* L, Context& context, const std::string& name, const std::string& value );

		static bool ReadBoolean( lua_State* L, Context& context, const std::string& name, bool& value, bool defaultValue );
		static bool WriteBoolean( lua_State* L, Context& context, const std::string& name, bool value );

	private:

		static Node* CreateAndReadFromTable( lua_State* L, Context& context );

		static bool AutoUpgradeIfNecessary( const std::string& file, Node*& node, Context& context );

		std::string name;
		List* children;
		Node* parent;
		MetaData* metaData;
		std::string explanation[ EXPLANATION_COUNT ];

		// Cornucopia trees should really never be so big that an
		// index is needed to make look-ups faster, but I'm going
		// to provide the feature anyway.  If you're doing many
		// lookups on a modestly sized tree, then utilizing the
		// index may give a significant performance improvement.
		// Using the index, however, can lead to a performance hit,
		// if you alternate between searches and structural tree
		// changes.  In that case, you would be rebuilding indices
		// every time you used them, which would be slow.
		class Index
		{
		public:
			
			Index( void );
			~Index( void );

			bool Lookup( List& nodeList, const std::string& name );
			bool IsValid( void );
			void Invalidate( void );
			void Rebuild( const Node* root );
			void Delete( void );

		private:

			std::map< std::string, List* > nodeMap;
			bool valid;
		};

		mutable Index* index;
	};
}

// Node.h