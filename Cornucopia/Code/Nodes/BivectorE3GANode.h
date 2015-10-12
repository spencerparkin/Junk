// BivectorE3GANode.h

namespace Cornucopia
{
	//=================================================================================
	// It is not hard to show that these are always blades.
	// This type of element can be used to represent rotations.
	// The unit-rotor is found by taking the exponent of the bivector.
	// So bivectors can also be used to represent angular velocity
	// and things like that.
	class CORNUCOPIA_API BivectorE3GANode : public Node
	{
		DECL_CORNUCOPIA_CLASSINFO( BivectorE3GANode );

	public:

		BivectorE3GANode( void );
		virtual ~BivectorE3GANode( void );

		virtual bool ReadFromTable( lua_State* L, Context& context ) override;
		virtual bool WriteToTable( lua_State* L, Context& context ) const override;

		virtual bool Copy( const Node* node, Context& context, const CopyParameters& copyParameters ) override;

		virtual std::string GetValueToDisplayString( void ) const override;
		virtual ValueStringError GetValueToString( std::string& valueString ) const override;
		virtual ValueStringError SetValueFromString( const std::string& valueString ) override;
		virtual ValueStringError GetValueAsStringHelp( std::string& helpString ) const override;

		c3ga::bivectorE3GA& GetBivector( void );
		const c3ga::bivectorE3GA& GetBivector( void ) const;
		bool SetBivector( const c3ga::bivectorE3GA& bivector, bool rescaleIfNeeded = false );

		double GetMaxArea( void ) const;
		void SetMaxArea( double maxArea );

	protected:

		double maxArea;
		c3ga::bivectorE3GA* bivector;
	};
}

// BivectorE3GANode.h