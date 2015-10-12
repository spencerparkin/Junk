// DataObject.h

namespace CornucopiaEditor
{
	//=================================================================================
	class DataObject : public wxDataObject
	{
	public:

		DataObject( void );
		virtual ~DataObject( void );

		bool SetNode( Cornucopia::Node* node );
		Cornucopia::Node* GetNode( void );

		virtual void GetAllFormats( wxDataFormat* formats, Direction dir = Get ) const override;
		virtual bool GetDataHere( const wxDataFormat& format, void* buf ) const override;
		virtual size_t GetDataSize( const wxDataFormat& format ) const override;
		virtual size_t GetFormatCount( Direction dir = Get ) const override;
		virtual wxDataFormat GetPreferredFormat( Direction dir = Get ) const override;
		virtual bool SetData( const wxDataFormat& format, size_t len, const void* buf ) override;

		static const wxString& GetFormatString( void );

	private:

		Cornucopia::Node* node;
		std::string luaBuffer;

		static wxString formatString;
	};
}

// DataObject.h