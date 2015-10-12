// ResourceManager.h

namespace CornucopiaEditor
{
	//=================================================================================
	class ResourceManager
	{
	public:

		ResourceManager( void );
		~ResourceManager( void );

		bool Initialize( int argc, char** argv );

		wxString FormulateFullPath( const wxString& relativePath );

		bool LoadImage( wxImage& image, const wxString& relativePath, wxBitmapType type = wxBITMAP_TYPE_ANY );
		bool LoadBitmap( wxBitmap& bitmap, const wxString& relativePath, wxBitmapType type = wxBITMAP_TYPE_ANY );

	private:

		wxString basePath;
	};
}

// ResourceManager.h