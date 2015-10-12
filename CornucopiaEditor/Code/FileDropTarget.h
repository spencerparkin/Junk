// FileDropTarget.h

namespace CornucopiaEditor
{
	//=================================================================================
	class FileDropTarget : public wxFileDropTarget
	{
	public:

		FileDropTarget( Frame* frame );
		virtual ~FileDropTarget( void );

		virtual bool OnDropFiles( wxCoord x, wxCoord y, const wxArrayString& fileNames ) override;

	private:

		Frame* frame;
	};
}

// FileDropTarget.h