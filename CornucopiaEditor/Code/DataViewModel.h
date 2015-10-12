// DataViewModel.h

namespace CornucopiaEditor
{
	//=================================================================================
	class DataViewModel : public wxDataViewModel
	{
		friend class DataViewCtrl;

	public:

		DataViewModel( Tree* tree, DataViewCtrl* dataViewCtrl );
		virtual ~DataViewModel( void );

		virtual wxDataViewItem GetParent( const wxDataViewItem& item ) const override;
		virtual unsigned int GetChildren( const wxDataViewItem& item, wxDataViewItemArray& children ) const override;

		virtual unsigned int GetColumnCount( void ) const override;
		virtual wxString GetColumnType( unsigned int col ) const override;
		
		virtual bool IsContainer( const wxDataViewItem& item ) const override;
		virtual bool HasContainerColumns( const wxDataViewItem& item ) const override;

		virtual void GetValue( wxVariant& variant, const wxDataViewItem& item, unsigned int col ) const override;
		virtual bool SetValue( const wxVariant& variant, const wxDataViewItem& item, unsigned int col ) override;

	private:

		enum
		{
			COL_NAME,
			COL_TYPE,
			COL_VALUE,
			COL_FEEDBACK,
			COL_UNUSED,		// This must be the last enum in the list!
		};

		Tree* tree;
		DataViewCtrl* dataViewCtrl;		// See comment in constructor.
	};
}

// DataViewModel.h