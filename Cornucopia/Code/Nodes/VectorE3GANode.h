// VectorE3GANode.h

namespace Cornucopia
{
	//=================================================================================
	class CORNUCOPIA_API VectorE3GANode : public Node
	{
		DECL_CORNUCOPIA_CLASSINFO( VectorE3GANode );

	public:

		VectorE3GANode( void );
		virtual ~VectorE3GANode( void );

		virtual bool ReadFromTable( lua_State* L, Context& context ) override;
		virtual bool WriteToTable( lua_State* L, Context& context ) const override;

		virtual bool Copy( const Node* node, Context& context, const CopyParameters& copyParameters ) override;

		virtual std::string GetValueToDisplayString( void ) const override;
		virtual ValueStringError GetValueToString( std::string& valueString ) const override;
		virtual ValueStringError SetValueFromString( const std::string& valueString ) override;
		virtual ValueStringError GetValueAsStringHelp( std::string& helpString ) const override;

		c3ga::vectorE3GA& GetVector( void );
		const c3ga::vectorE3GA& GetVector( void ) const;
		bool SetVector( const c3ga::vectorE3GA& vector, bool rescaleIfNeeded = false );

		double GetMaxLength( void ) const;
		void SetMaxLength( double maxLength );

	protected:

		double maxLength;
		c3ga::vectorE3GA* vector;
	};
}

// VectorE3GANode.h