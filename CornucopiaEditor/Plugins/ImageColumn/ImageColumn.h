// ImageColumn.h

#include "Plugin.h"

//=================================================================================
class ImageColumnPlugin : public CornucopiaEditor::Plugin
{
public:

	ImageColumnPlugin( void );
	virtual ~ImageColumnPlugin();

	virtual bool Initialize( CornucopiaEditor::PluginAssistant* assistant ) override;
	virtual void Finalize( CornucopiaEditor::PluginAssistant* assistant ) override;

	virtual void ProvideInfo( Info& pluginInfo ) override;

	void OnAugmentPanelUI( CornucopiaEditor::PluginPageEvent& event );

	void OnGetDataViewControlValue( CornucopiaEditor::PluginDataViewCtrlEvent& event );
	void OnSetDataViewControlValue( CornucopiaEditor::PluginDataViewCtrlEvent& event );

	void OnRefreshProperty( CornucopiaEditor::PluginPropertyEvent& event );

private:

	wxDataViewCtrl* FindDataViewCtrlForTree( wxPanel* panel );

	void AddAllImageColumns( void );
	void RemoveAllImageColumns( void );

	void AddImageColumn( wxPanel* panel );
	void RemoveImageColumn( wxPanel* panel );

	wxDataViewColumn* FindImageColumn( wxDataViewCtrl* dataViewCtrl );

	bool IsColumnOwner( wxDataViewCtrl* dataViewCtrl, unsigned int modelCol );

	bool GetBitmapForFile( const Cornucopia::Path& filePath, wxBitmap& bitmap );
	bool GetBitmapForNode( const Cornucopia::Node* node, wxBitmap& bitmap );

	wxBitmap nullBitmap;

	void WipeBitmapCache( void );
	bool CheckBitmapCache( const Cornucopia::Path& filePath, wxBitmap& bitmap );
	void AddToBitmapCache( const Cornucopia::Path& filePath, wxBitmap& bitmap );

	typedef std::map< std::string, wxBitmap > BitmapCache;
	BitmapCache bitmapCache;

	CornucopiaEditor::PluginAssistant* assistant;
};

// ImageColumn.h