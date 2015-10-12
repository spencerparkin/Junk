// FileReferenceNode.h

namespace Cornucopia
{
	//=================================================================================
	class CORNUCOPIA_API FileReferenceNode : public Node
	{
		DECL_CORNUCOPIA_CLASSINFO( FileReferenceNode );

	public:

		FileReferenceNode( void );
		virtual ~FileReferenceNode( void );

		virtual bool ReadFromTable( lua_State* L, Context& context ) override;
		virtual bool WriteToTable( lua_State* L, Context& context ) const override;

		virtual bool Copy( const Node* node, Context& context, const CopyParameters& copyParameters ) override;

		bool SetFilePath( const Path& filePath );
		bool GetFilePath( Path& filePath ) const;

		static bool SetBaseDir( const Path& baseDir );
		static bool GetBaseDir( Path& baseDir );

		virtual std::string GetValueToDisplayString( void ) const override;
		virtual ValueStringError GetValueToString( std::string& valueString ) const override;
		virtual ValueStringError SetValueFromString( const std::string& valueString ) override;
		virtual ValueStringError GetValueAsStringHelp( std::string& helpString ) const override;

	private:

		Path filePath;
		static Path baseDir;
	};
}

// FileReferenceNode.h