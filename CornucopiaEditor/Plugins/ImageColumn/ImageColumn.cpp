// ImageColumn.cpp

#include "ImageColumn.h"

//=================================================================================
// It seems like a contradiction that a function with extern "C" can call C++ code.
extern "C" __declspec( dllexport ) CornucopiaEditor::Plugin* CreatePlugin( void )
{
	return new ImageColumnPlugin();
}

//=================================================================================
ImageColumnPlugin::ImageColumnPlugin( void )
{
	wxASSERT( nullBitmap.IsNull() );
}

//=================================================================================
/*virtual*/ ImageColumnPlugin::~ImageColumnPlugin()
{
	WipeBitmapCache();
}

//=================================================================================
/*virtual*/ bool ImageColumnPlugin::Initialize( CornucopiaEditor::PluginAssistant* assistant )
{
	// It is safe to cache this pointer.
	this->assistant = assistant;

	Bind( CornucopiaEditor::CORNUCOPIA_PLUGIN_PAGE_AUGMENT_UI, &ImageColumnPlugin::OnAugmentPanelUI, this );
	Bind( CornucopiaEditor::CORNUCOPIA_PLUGIN_DATAVIEWCTRL_GET_VALUE, &ImageColumnPlugin::OnGetDataViewControlValue, this );
	Bind( CornucopiaEditor::CORNUCOPIA_PLUGIN_DATAVIEWCTRL_SET_VALUE, &ImageColumnPlugin::OnSetDataViewControlValue, this );
	Bind( CornucopiaEditor::CORNUCOPIA_PLUGIN_PROPERTY_REFRESH, &ImageColumnPlugin::OnRefreshProperty, this );

	AddAllImageColumns();

	return true;
}

//=================================================================================
/*virtual*/ void ImageColumnPlugin::Finalize( CornucopiaEditor::PluginAssistant* assistant )
{
	RemoveAllImageColumns();

	WipeBitmapCache();
}

//=================================================================================
void ImageColumnPlugin::AddAllImageColumns( void )
{
	wxAuiNotebook* noteBook = assistant->GetAuiNoteBook();
	for( unsigned int index = 0; index < noteBook->GetPageCount(); index++ )
	{
		wxWindow* page = noteBook->GetPage( index );
		wxPanel* panel = wxDynamicCast( page, wxPanel );
		if( panel )
			AddImageColumn( panel );
	}
}

//=================================================================================
void ImageColumnPlugin::RemoveAllImageColumns( void )
{
	wxAuiNotebook* noteBook = assistant->GetAuiNoteBook();
	for( unsigned int index = 0; index < noteBook->GetPageCount(); index++ )
	{
		wxWindow* page = noteBook->GetPage( index );
		wxPanel* panel = wxDynamicCast( page, wxPanel );
		if( panel )
			RemoveImageColumn( panel );
	}
}

//=================================================================================
void ImageColumnPlugin::AddImageColumn( wxPanel* panel )
{
	wxDataViewCtrl* dataViewCtrl = FindDataViewCtrlForTree( panel );
	if( dataViewCtrl )
	{
		int modelCol = assistant->GetUnusedModelColumn( dataViewCtrl );
		if( modelCol >= 0 )
			dataViewCtrl->AppendBitmapColumn( wxT( "Image" ), modelCol, wxDATAVIEW_CELL_INERT, -1, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE );
	}
}

//=================================================================================
void ImageColumnPlugin::RemoveImageColumn( wxPanel* panel )
{
	wxDataViewCtrl* dataViewCtrl = FindDataViewCtrlForTree( panel );
	if( dataViewCtrl )
	{
		wxDataViewColumn* dataViewColumn = FindImageColumn( dataViewCtrl );
		if( dataViewColumn )
			dataViewCtrl->DeleteColumn( dataViewColumn );
	}
}

//=================================================================================
// We'll have a problem if the application already defines a column with header title "Image".
wxDataViewColumn* ImageColumnPlugin::FindImageColumn( wxDataViewCtrl* dataViewCtrl )
{
	wxDataViewColumn* foundDataViewColumn = 0;
	for( unsigned int index = 0; index < dataViewCtrl->GetColumnCount() && !foundDataViewColumn; index++ )
	{
		wxDataViewColumn* dataViewColumn = dataViewCtrl->GetColumn( index );
		if( dataViewColumn->GetTitle() == wxT( "Image" ) )
			foundDataViewColumn = dataViewColumn;
	}
	return foundDataViewColumn;
}

//=================================================================================
/*virtual*/ void ImageColumnPlugin::ProvideInfo( Info& pluginInfo )
{
	pluginInfo.pluginAuthor = wxT( "Spencer T. parkin" );
	pluginInfo.pluginName = wxT( "ImageColumn" );
	pluginInfo.pluginVersion = wxT( "v1.0" );
	pluginInfo.pluginDesc = wxT( "Provides an extra column that displays images for file reference nodes of image files." );
}

//=================================================================================
void ImageColumnPlugin::OnAugmentPanelUI( CornucopiaEditor::PluginPageEvent& event )
{
	wxWindow* page = event.GetPage();
	wxPanel* panel = wxDynamicCast( page, wxPanel );
	if( panel )
		AddImageColumn( panel );

	// Let other plugins augment the panel UI too.
	event.Skip();
}

//=================================================================================
wxDataViewCtrl* ImageColumnPlugin::FindDataViewCtrlForTree( wxPanel* panel )
{
	// It would really be better if the application gave names to the controls
	// so that we could search for them by name.  Here, the plugin must have
	// some knowledge of the application's control hierarchy.
	const wxWindowList& windowList = panel->GetChildren();
	wxDataViewCtrl* dataViewCtrl = 0;
	for( unsigned int index = 0; index < windowList.size() && !dataViewCtrl; index++ )
	{
		wxWindow* child = windowList[ index ];
		dataViewCtrl = wxDynamicCast( child, wxDataViewCtrl );
		if( !assistant->IsDataViewCtrlForTree( dataViewCtrl ) )
			dataViewCtrl = 0;
	}
	return dataViewCtrl;
}

//=================================================================================
bool ImageColumnPlugin::IsColumnOwner( wxDataViewCtrl* dataViewCtrl, unsigned int modelCol )
{
	wxDataViewColumn* dataViewColumn = FindImageColumn( dataViewCtrl );
	if( !dataViewColumn || dataViewColumn->GetModelColumn() != modelCol )
		return false;
	return true;
}

//=================================================================================
void ImageColumnPlugin::OnRefreshProperty( CornucopiaEditor::PluginPropertyEvent& event )
{
	Cornucopia::Node* node = event.GetNode();
	Cornucopia::FileReferenceNode* fileReferenceNode = node->Cast< Cornucopia::FileReferenceNode >();
	if( !node )
	{
		event.Skip();
		return;
	}

	wxBitmap bitmap;
	if( !GetBitmapForNode( node, bitmap ) )
		bitmap = nullBitmap;

	wxPGProperty* property = event.GetProperty();
	property->SetValueImage( bitmap );
}

//=================================================================================
void ImageColumnPlugin::OnGetDataViewControlValue( CornucopiaEditor::PluginDataViewCtrlEvent& event )
{
	wxDataViewCtrl* dataViewCtrl = event.GetDataViewCtrl();
	wxDataViewItem item = event.GetDataViewItem();
	wxVariant& variant = event.GetVariant();
	unsigned int col = event.GetColumn();

	// Is it the column we own?
	if( !IsColumnOwner( dataViewCtrl, col ) )
	{
		event.Skip();
		return;
	}

	// Yes.  If this turns out to be a non-image related node, then we'll use the null-bitmap.
	variant << nullBitmap;

	// Can we get a bitmap associated with the node for this item?
	wxBitmap bitmap;
	const Cornucopia::Node* node = ( const Cornucopia::Node* )item.m_pItem;
	if( !GetBitmapForNode( node, bitmap ) )
		return;		// No, just keep the null bitmap.

	// Yes, return the bitmap image.
	variant << bitmap;
}

//=================================================================================
void ImageColumnPlugin::OnSetDataViewControlValue( CornucopiaEditor::PluginDataViewCtrlEvent& event )
{
	wxDataViewCtrl* dataViewCtrl = event.GetDataViewCtrl();
	wxDataViewItem item = event.GetDataViewItem();
	const wxVariant& variant = event.GetVariant();
	unsigned int col = event.GetColumn();

	// Is it the column we own?
	if( !IsColumnOwner( dataViewCtrl, col ) )
	{
		event.Skip();
		return;
	}

	// Do nothing.
	return;
}

//=================================================================================
bool ImageColumnPlugin::GetBitmapForNode( const Cornucopia::Node* node, wxBitmap& bitmap )
{
	// Are we dealing with a file reference node here?
	const Cornucopia::FileReferenceNode* fileReferenceNode = node->Cast< Cornucopia::FileReferenceNode >();
	if( !fileReferenceNode )
		return false;

	// Yes.  What's the file path?
	Cornucopia::Path filePath;
	if( !fileReferenceNode->GetFilePath( filePath ) )
		return false;

	// If the file is an image file, get the image as a bitmap.
	if( !GetBitmapForFile( filePath, bitmap ) )
		return false;

	return true;
}

//=================================================================================
bool ImageColumnPlugin::GetBitmapForFile( const Cornucopia::Path& filePath, wxBitmap& bitmap )
{
	// We can't read nothing.
	if( filePath.IsNull() || !filePath.Exists() )
		return false;

	// Can we read the file as an image?  Note that the file must exist before issuing this test, or we get an error.
	if( !wxImage::CanRead( ( const char* )filePath ) )
		return false;

	// Yes.  It it already in the cache?  (If so, return the cached image.)
	if( CheckBitmapCache( filePath, bitmap ) )
		return true;

	// No.  Okay, go load the image up.
	wxBusyCursor busyCursor;
	wxImage image;
	if( !image.LoadFile( ( const char* )filePath ) )
		return false;
	
	// Make the image look nice and turn it into a bitmap.
	image.Rescale( 22, 22, wxIMAGE_QUALITY_HIGH );
	bitmap = wxBitmap( image );

	// Add it to the cache so that we don't have to prepare it again.
	AddToBitmapCache( filePath, bitmap );

	// Hurrah!
	return true;
}

//=================================================================================
void ImageColumnPlugin::WipeBitmapCache( void )
{
	bitmapCache.clear();
}

//=================================================================================
bool ImageColumnPlugin::CheckBitmapCache( const Cornucopia::Path& filePath, wxBitmap& bitmap )
{
	std::string key;
	filePath.GetPath( key, true );
	BitmapCache::iterator iter = bitmapCache.find( key );
	if( iter == bitmapCache.end() )
		return false;
	bitmap = iter->second;
	return true;
}

//=================================================================================
void ImageColumnPlugin::AddToBitmapCache( const Cornucopia::Path& filePath, wxBitmap& bitmap )
{
	std::string key;
	filePath.GetPath( key, true );
	bitmapCache[ key ] = bitmap;
}

// ImageColumn.cpp