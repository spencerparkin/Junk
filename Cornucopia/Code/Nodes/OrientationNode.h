// OrientationNode.h

namespace Cornucopia
{
	//=================================================================================
	class CORNUCOPIA_API OrientationNode : public Node
	{
		DECL_CORNUCOPIA_CLASSINFO( OrientationNode );

	public:

		OrientationNode( void );
		virtual ~OrientationNode( void );

		virtual bool ReadFromTable( lua_State* L, Context& context ) override;
		virtual bool WriteToTable( lua_State* L, Context& context ) const override;

		virtual bool Copy( const Node* node, Context& context, const CopyParameters& copyParameters ) override;

		virtual std::string GetValueToDisplayString( void ) const override;
		virtual ValueStringError GetValueToString( std::string& valueString ) const override;
		virtual ValueStringError SetValueFromString( const std::string& valueString ) override;
		virtual ValueStringError GetValueAsStringHelp( std::string& helpString ) const override;

		const c3ga::rotorE3GA& GetRotor( void ) const;
		void SetRotor( c3ga::rotorE3GA& rotor );	// We assume here that it's of unit magnitude.

	protected:

		c3ga::rotorE3GA* rotor;
	};
}

// OrientationNode.h