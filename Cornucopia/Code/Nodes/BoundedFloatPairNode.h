// BoundedFloatPairNode.h

namespace Cornucopia
{
	//=================================================================================
	// This is like the bounded float node, but maintains a pair of values constrained
	// in such a way that one must always be less then or equal to the other.  You could
	// think of it as a bounded compact interval.
	class CORNUCOPIA_API BoundedFloatPairNode : public Node
	{
		DECL_CORNUCOPIA_CLASSINFO( BoundedFloatPairNode );

	public:

		BoundedFloatPairNode( void );
		virtual ~BoundedFloatPairNode( void );

		virtual bool ReadFromTable( lua_State* L, Context& context ) override;
		virtual bool WriteToTable( lua_State* L, Context& context ) const override;

		virtual bool Copy( const Node* node, Context& context, const CopyParameters& copyParameters ) override;

		struct Data
		{
			double min, max;
			double minValue, maxValue;

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

// BoundedFloatPairNode.h