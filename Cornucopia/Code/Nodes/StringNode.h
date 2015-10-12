// StringNode.h

namespace Cornucopia
{
	//=================================================================================
	class CORNUCOPIA_API StringNode : public Node
	{
		DECL_CORNUCOPIA_CLASSINFO( StringNode );

	public:

		StringNode( void );
		virtual ~StringNode( void );

		virtual bool ReadFromTable( lua_State* L, Context& context ) override;
		virtual bool WriteToTable( lua_State* L, Context& context ) const override;

		virtual bool Copy( const Node* node, Context& context, const CopyParameters& copyParameters ) override;

		bool SetString( const std::string& string );
		bool GetString( std::string& string ) const;

		virtual std::string GetValueToDisplayString( void ) const override;
		virtual ValueStringError GetValueToString( std::string& valueString ) const override;
		virtual ValueStringError SetValueFromString( const std::string& valueString ) override;
		virtual ValueStringError GetValueAsStringHelp( std::string& helpString ) const override;

	protected:

		std::string string;
	};
}

// StringNode.h