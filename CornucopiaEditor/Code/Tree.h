// Tree.h

namespace CornucopiaEditor
{
	//=================================================================================
	// This class represents the data that is edited by this editor.
	// TODO: We can't use the file watcher for now, because 1) it's not fully supported
	//       in wxWidgets, and 2) on shutdown, it does a thread wait which lets some
	//       events fire that try to use memory that's already been freed.
	class Tree //: public wxFileSystemWatcher
	{
	public:

		Tree( void );
		virtual ~Tree( void );

		void Delete( void );

		bool Open( const wxString& filePath );
		bool Save( void );
		bool Close( void );

		Cornucopia::Node* GetRoot( void );
		bool SetRoot( Cornucopia::Node* root );

		bool GetModified( void );
		void SetModified( bool modified );

		wxString GetTitle( void );

		wxString GetFilePath( void );
		void SetFilePath( const wxString& filePath );

		void PopulateNonUniqueNameList( std::list< std::string >& nonUniqueNameList );

	private:

		wxString filePath;
		Cornucopia::Node* root;
		bool modified;
		int untitledNumber;

		static int nextUntitledNumber;
	};
}

// Tree.h