// SelectionNode.h

namespace Cornucopia
{
	//=================================================================================
	// A selection node maintains a set of items from which one can select.
	// If in single select mode, one and only one of the items can be selected.
	// In multi-select mode, zero or more of the items can be selected.
	class CORNUCOPIA_API SelectionNode : public Node
	{
		DECL_CORNUCOPIA_CLASSINFO( SelectionNode );

	public:

		SelectionNode( void );
		virtual ~SelectionNode( void );

		virtual bool ReadFromTable( lua_State* L, Context& context ) override;
		virtual bool WriteToTable( lua_State* L, Context& context ) const override;

		virtual bool Copy( const Node* node, Context& context, const CopyParameters& copyParameters ) override;

		virtual std::string GetValueToDisplayString( void ) const override;
		virtual ValueStringError GetValueToString( std::string& valueString ) const override;
		virtual ValueStringError SetValueFromString( const std::string& valueString ) override;
		virtual ValueStringError GetValueAsStringHelp( std::string& helpString ) const override;

		enum Mode
		{
			MULTI_SELECT,
			SINGLE_SELECT,
		};

		Mode GetMode( void ) const;
		bool SetMode( Mode mode );

		bool Select( const std::string& selectionName, bool selected, bool addIfNotFound = false );
		bool IsSelected( const std::string& selectionName ) const;
		bool SelectionStateValid( void ) const;
		int SelectionCount( void ) const;
		int SelectableCount( void ) const;

		typedef std::map< std::string, bool > SelectionMap;
		const SelectionMap& GetSelectionMap( void ) const;

	private:

		SelectionMap* selectionMap;
		Mode mode;
	};
}

// SelectionNode.h