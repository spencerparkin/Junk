// ConeNode.h

namespace Cornucopia
{
	//=================================================================================
	class CORNUCOPIA_API ConeNode : public VectorE3GANode
	{
		DECL_CORNUCOPIA_CLASSINFO( ConeNode );

	public:

		ConeNode( void );
		virtual ~ConeNode( void );

		virtual bool ReadFromTable( lua_State* L, Context& context ) override;
		virtual bool WriteToTable( lua_State* L, Context& context ) const override;

		virtual bool Copy( const Node* node, Context& context, const CopyParameters& copyParameters ) override;

		virtual std::string GetValueToDisplayString( void ) const override;
		virtual ValueStringError GetValueToString( std::string& valueString ) const override;
		virtual ValueStringError SetValueFromString( const std::string& valueString ) override;
		virtual ValueStringError GetValueAsStringHelp( std::string& helpString ) const override;

		bool SetConeAngle( double coneAngle );
		double GetConeAngle( void ) const;

	protected:

		double coneAngle;
	};
}

// ConeNode.h