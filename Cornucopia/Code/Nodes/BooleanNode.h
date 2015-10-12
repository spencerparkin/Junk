// BooleanNode.h

namespace Cornucopia
{
	//=================================================================================
	class CORNUCOPIA_API BooleanNode : public Node
	{
		DECL_CORNUCOPIA_CLASSINFO( BooleanNode );

	public:

		BooleanNode( void );
		virtual ~BooleanNode( void );

		virtual bool ReadFromTable( lua_State* L, Context& context ) override;
		virtual bool WriteToTable( lua_State* L, Context& context ) const override;

		virtual bool Copy( const Node* node, Context& context, const CopyParameters& copyParameters ) override;

		void SetBoolean( bool value );
		bool GetBoolean( void ) const;

		virtual std::string GetValueToDisplayString( void ) const override;
		virtual ValueStringError GetValueToString( std::string& valueString ) const override;
		virtual ValueStringError SetValueFromString( const std::string& valueString ) override;
		virtual ValueStringError GetValueAsStringHelp( std::string& helpString ) const override;

	private:

		bool value;
	};
}

// BooleanNode.h