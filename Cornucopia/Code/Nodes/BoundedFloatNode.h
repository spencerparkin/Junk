// BoundedFloatNode.h

namespace Cornucopia
{
	//=================================================================================
	class CORNUCOPIA_API BoundedFloatNode : public Node
	{
		DECL_CORNUCOPIA_CLASSINFO( BoundedFloatNode );

	public:

		BoundedFloatNode( void );
		virtual ~BoundedFloatNode( void );

		virtual bool ReadFromTable( lua_State* L, Context& context ) override;
		virtual bool WriteToTable( lua_State* L, Context& context ) const override;

		virtual bool Copy( const Node* node, Context& context, const CopyParameters& copyParameters ) override;

		struct CORNUCOPIA_API Data
		{
			double min, max;
			double value;

			bool IsValid( void ) const;
			bool operator==( const Data& data ) const;
		};

		bool Set( const Data& data );
		void Get( Data& data ) const;

		virtual std::string GetValueToDisplayString( void ) const override;
		virtual ValueStringError GetValueToString( std::string& valueString ) const override;
		virtual ValueStringError SetValueFromString( const std::string& valueString ) override;
		virtual ValueStringError GetValueAsStringHelp( std::string& helpString ) const override;

	private:

		Data data;
	};
}

// BoundedFloatNode.h