// ColorRGBANode.h

namespace Cornucopia
{
	//=================================================================================
	class CORNUCOPIA_API ColorRGBANode : public Node
	{
		DECL_CORNUCOPIA_CLASSINFO( ColorRGBANode );

	public:

		ColorRGBANode( void );
		virtual ~ColorRGBANode( void );

		virtual bool ReadFromTable( lua_State* L, Context& context ) override;
		virtual bool WriteToTable( lua_State* L, Context& context ) const override;

		virtual bool Copy( const Node* node, Context& context, const CopyParameters& copyParameters ) override;

		virtual std::string GetValueToDisplayString( void ) const override;
		virtual ValueStringError GetValueToString( std::string& valueString ) const override;
		virtual ValueStringError SetValueFromString( const std::string& valueString ) override;
		virtual ValueStringError GetValueAsStringHelp( std::string& helpString ) const override;

		class CORNUCOPIA_API Color
		{
		public:

			Color( void );
			Color( const Color& color );

			Color& operator=( const Color& color );

			bool operator==( const Color& color ) const;

			double Brightness( void ) const;

			bool IsValid( void ) const;

			double r, g, b, a;
		};

		bool SetColor( const Color& color );
		bool GetColor( Color& color ) const;

	protected:

		Color color;
	};
}

// ColorRGBANode.h