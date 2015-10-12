// Upgrader.h

namespace CornucopiaEditor
{
	//=================================================================================
	class Upgrader : public wxDirTraverser
	{
	public:

		Upgrader( void );
		virtual ~Upgrader( void );

		virtual wxDirTraverseResult OnDir( const wxString& dirName ) override;
		virtual wxDirTraverseResult OnFile( const wxString& fileName ) override;
		virtual wxDirTraverseResult OnOpenError( const wxString& openErrorName ) override;

		void PerformUpgrade( void );

		const wxString& GetReport( void );

	private:

		wxArrayString fileList;
		wxString report;
	};
}

// Upgrader.h