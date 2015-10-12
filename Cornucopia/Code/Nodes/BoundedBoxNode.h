// BoundedBoxNode.h

namespace Cornucopia
{
	//=================================================================================
	class CORNUCOPIA_API BoundedBoxNode : public Node
	{
		DECL_CORNUCOPIA_CLASSINFO( BoundedBoxNode );

	public:

		BoundedBoxNode( void );
		virtual ~BoundedBoxNode( void );

		virtual bool ReadFromTable( lua_State* L, Context& context ) override;
		virtual bool WriteToTable( lua_State* L, Context& context ) const override;

		virtual bool Copy( const Node* node, Context& context, const CopyParameters& copyParameters ) override;

		virtual std::string GetValueToDisplayString( void ) const override;
		virtual ValueStringError GetValueToString( std::string& valueString ) const override;
		virtual ValueStringError SetValueFromString( const std::string& valueString ) override;
		virtual ValueStringError GetValueAsStringHelp( std::string& helpString ) const override;

		struct Box
		{
			c3ga::vectorE3GA min, max;

			bool IsValid( void ) const;
			bool Contains( const Box& box ) const;
			bool Contains( const c3ga::vectorE3GA& point ) const;
			bool operator==( const Box& box ) const;
		};

		struct Data
		{
			Box box;
			Box boundingBox;

			bool IsValid( void ) const;
			bool operator==( const Data& data ) const;
		};

		bool Set( const Data& data );
		void Get( Data& data ) const;

	protected:

		Data data;
	};
}

// BoundedBoxNode.h