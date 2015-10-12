// OriginReferenceNode.h

namespace Cornucopia
{
	//=================================================================================
	// The origin reference node makes sense only as the root node of a tree.
	// Cornucopia recognizes nodes of this type as indicating what tree, if any,
	// a given tree was cloned from.  If the tree in question has an origin
	// reference root node that points to itself, then it is considered to be
	// a template tree, which is one from which other trees are cloned, and
	// therefore also defines a "file format", if you will.  The original
	// tree serves as a template for other tree instances and for storing default values.
	// If the original tree changes in value or structure, Cornucopia can be used
	// to upgrade any tree that was originally cloned from it.
	class CORNUCOPIA_API OriginReferenceNode : public FileReferenceNode
	{
		DECL_CORNUCOPIA_CLASSINFO( OriginReferenceNode );

	public:

		OriginReferenceNode( void );
		virtual ~OriginReferenceNode( void );

		virtual bool ReadFromTable( lua_State* L, Context& context ) override;
		virtual bool WriteToTable( lua_State* L, Context& context ) const override;

		virtual bool Copy( const Node* node, Context& context, const CopyParameters& copyParameters ) override;

		virtual std::string GetValueToDisplayString( void ) const override;
		virtual ValueStringError GetValueToString( std::string& valueString ) const override;
		virtual ValueStringError SetValueFromString( const std::string& valueString ) override;
		virtual ValueStringError GetValueAsStringHelp( std::string& helpString ) const override;

		void SetAutoUpgrade( bool autoUpgrade );
		bool GetAutoUpgrade( void ) const;

		void SetVersion( double version );
		double GetVersion( void ) const;

	private:

		double version;

		// If true, Cornucopia will attempt to upgrade the tree every time
		// the tree is loaded from file.
		bool autoUpgrade;
	};
}

// OriginReferenceNode.h