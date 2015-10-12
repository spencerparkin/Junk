// BoundedIntegerNode.h

namespace Cornucopia
{
	//=================================================================================
	class CORNUCOPIA_API BoundedIntegerNode : public Node
	{
		DECL_CORNUCOPIA_CLASSINFO( BoundedIntegerNode );

	public:

		BoundedIntegerNode( void );
		virtual ~BoundedIntegerNode( void );

		virtual bool ReadFromTable( lua_State* L, Context& context ) override;
		virtual bool WriteToTable( lua_State* L, Context& context ) const override;

		virtual bool Copy( const Node* node, Context& context, const CopyParameters& copyParameters ) override;

		struct CORNUCOPIA_API Data
		{
			int min, max;
			int value;
			int multiple;

			bool IsValid( void ) const;
			bool operator==( const Data& data ) const;
			int Cardinality( void ) const;
		};

		bool Set( const Data& data );
		void Get( Data& data ) const;

		virtual std::string GetValueToDisplayString( void ) const override;
		virtual ValueStringError GetValueToString( std::string& valueString ) const override;
		virtual ValueStringError SetValueFromString( const std::string& valueString ) override;
		virtual ValueStringError GetValueAsStringHelp( std::string& helpString ) const override;

		static int RoundDownToNearestMultiple( int integer, int multiple );
		static int RoundUpToNearestMultiple( int integer, int multiple );
		static int RoundToNearestMultiple( int integer, int multiple );

	private:

		Data data;
	};
}

// BoundedIntegerNode.h