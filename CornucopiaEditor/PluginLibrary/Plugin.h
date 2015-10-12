// Plugin.h

#ifndef __CORNUCOPIA_PLUGIN__
#define __CORNUCOPIA_PLUGIN__

#if defined( CORNUCOPIA_PLUGIN_DLL_EXPORTS )
#	define CORNUCOPIA_PLUGIN_API __declspec( dllexport )
#elif defined( CORNCUOPIA_PLUGIN_DLL_IMPORTS )
#	define CORNUCOPIA_PLUGIN_API __declspec( dllimport )
#else
#	define CORNUCOPIA_PLUGIN_API
#endif

//=================================================================================
// This header file is shared between the Corncuopia Editor application and
// any plugin written for it.  A written plugin need only include this header,
// dynamically link against the plugin shared library, against the Cornucopia
// shared library, and against wxWidgets as a shared library.  The Cornucopia
// Editor can then load the plugin and send events to it.  A plugin for the
// Cornucopia Editor is implemented as an event handler that responds to events
// sent from the application.  The plugin library must expose a function named
// "CreatePlugin" that, when called by the application, allocates and returns an
// instance of the plugin's event handler, which must be a derivative of
// CornucopiaEditor::Plugin as defined below.  Also defined below are the events
// currently supported/exposed by the Cornucopia Editor application.
//=================================================================================

#include <wx/setup.h>
#include <wx/app.h>
#include <wx/frame.h>
#include <wx/dataview.h>
#include <wx/utils.h>
#include <wx/aui/aui.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>

#include "Cornucopia.h"

namespace CornucopiaEditor
{
	class Plugin;

	//=================================================================================
	// Given to all plugins for each event, this class can be used to interface
	// with the Cornucopia Editor application to get useful information and to
	// request the application perform certain tasks on behalf of the plugin.
	class PluginAssistant
	{
	public:

		virtual wxFrame* GetFrame( void ) = 0;
		virtual wxAuiManager* GetFrameAuiManager( void ) = 0;
		virtual wxAuiNotebook* GetAuiNoteBook( void ) = 0;

		virtual bool IsDataViewCtrlForTree( wxDataViewCtrl* dataViewCtrl ) = 0;
		virtual int GetUnusedModelColumn( wxDataViewCtrl* dataViewCtrl ) = 0;
		
		virtual wxWindow* GetCurrentPage( void ) = 0;
		virtual Cornucopia::Node* GetCurrentTree( void ) = 0;
		virtual wxString GetCurrentFile( void ) = 0;

		virtual Cornucopia::Node* GetTreeFromPage( wxWindow* page ) = 0;
		virtual wxString GetFileFromPage( wxWindow* page ) = 0;

		virtual bool RequestPageReload( wxWindow* page, const wxString& reason ) = 0;

		virtual bool UnloadPlugin( const wxString& libraryPath ) = 0;
	};

	//=================================================================================
	class CORNUCOPIA_PLUGIN_API PluginEvent : public wxEvent
	{
	public:
		PluginEvent( int winid = 0, wxEventType commandType = wxEVT_NULL );
		virtual ~PluginEvent( void );

		virtual wxEvent* Clone( void ) const override;

		void SetAssistant( PluginAssistant* assistant ) { this->assistant = assistant; }
		PluginAssistant* GetAssistant( void ) { return assistant; }

		void SetLibraryPath( const wxString& libraryPath ) { this->libraryPath = libraryPath; }
		wxString GetLibraryPath( void ) { return libraryPath; }

	protected:

		PluginAssistant* assistant;
		wxString libraryPath;
	};

	//=================================================================================
	wxDECLARE_EXPORTED_EVENT( CORNUCOPIA_PLUGIN_API, CORNUCOPIA_PLUGIN_INITIALIZE, PluginEvent );
	wxDECLARE_EXPORTED_EVENT( CORNUCOPIA_PLUGIN_API, CORNUCOPIA_PLUGIN_FINALIZE, PluginEvent );

	//=================================================================================
	class CORNUCOPIA_PLUGIN_API PluginPageEvent : public PluginEvent
	{
	public:

		PluginPageEvent( int winid = 0, wxEventType commandType = wxEVT_NULL );
		virtual ~PluginPageEvent( void );

		virtual wxEvent* Clone( void ) const override;

		void SetPage( wxWindow* page ) { this->page = page; }
		wxWindow* GetPage( void ) { return page; }

	private:

		wxWindow* page;
	};

	//=================================================================================
	wxDECLARE_EXPORTED_EVENT( CORNUCOPIA_PLUGIN_API, CORNUCOPIA_PLUGIN_PAGE_CREATED, PluginPageEvent );
	wxDECLARE_EXPORTED_EVENT( CORNUCOPIA_PLUGIN_API, CORNUCOPIA_PLUGIN_PAGE_CHANGED, PluginPageEvent );
	wxDECLARE_EXPORTED_EVENT( CORNUCOPIA_PLUGIN_API, CORNUCOPIA_PLUGIN_PAGE_DESTROYING, PluginPageEvent );
	wxDECLARE_EXPORTED_EVENT( CORNUCOPIA_PLUGIN_API, CORNUCOPIA_PLUGIN_PAGE_AUGMENT_UI, PluginPageEvent );

	//=================================================================================
	class CORNUCOPIA_PLUGIN_API PluginDataViewCtrlEvent : public PluginEvent
	{
	public:

		PluginDataViewCtrlEvent( int winid = 0, wxEventType commandType = wxEVT_NULL );
		virtual ~PluginDataViewCtrlEvent( void );

		virtual wxEvent* Clone( void ) const override;

		void SetDataViewCtrl( wxDataViewCtrl* dataViewCtrl ) { this->dataViewCtrl = dataViewCtrl; }
		wxDataViewCtrl* GetDataViewCtrl( void ) { return dataViewCtrl; }

		void SetDataViewItem( const wxDataViewItem& dataViewItem ) { this->dataViewItem = dataViewItem; }
		const wxDataViewItem& GetDataViewItem( void ) { return dataViewItem; }

		void SetVariant( const wxVariant& variant ) { this->variant = variant; }
		wxVariant& GetVariant( void ) { return variant; }

		void SetColumn( unsigned int col ) { this->col = col; }
		unsigned int GetColumn( void ) { return col; }

	private:

		wxDataViewCtrl* dataViewCtrl;
		wxDataViewItem dataViewItem;
		wxVariant variant;
		unsigned int col;
	};

	//=================================================================================
	wxDECLARE_EXPORTED_EVENT( CORNUCOPIA_PLUGIN_API, CORNUCOPIA_PLUGIN_DATAVIEWCTRL_GET_VALUE, PluginDataViewCtrlEvent );
	wxDECLARE_EXPORTED_EVENT( CORNUCOPIA_PLUGIN_API, CORNUCOPIA_PLUGIN_DATAVIEWCTRL_SET_VALUE, PluginDataViewCtrlEvent );

	//=================================================================================
	class CORNUCOPIA_PLUGIN_API PluginPropertyEvent : public PluginEvent
	{
	public:

		PluginPropertyEvent( int winid = 0, wxEventType commandType = wxEVT_NULL );
		virtual ~PluginPropertyEvent( void );

		virtual wxEvent* Clone( void ) const override;

		void SetProperty( wxPGProperty* property ) { this->property = property; }
		wxPGProperty* GetProperty( void ) { return property; }

		void SetKeyColumn( int keyColumn ) { this->keyColumn = keyColumn; }
		int GetKeyColumn( void ) { return keyColumn; }

		void SetValueColumn( int valueColumn ) { this->valueColumn = valueColumn; }
		int GetValueColumn( void ) { return valueColumn; }

		void SetNode( Cornucopia::Node* node ) { this->node = node; }
		Cornucopia::Node* GetNode( void ) { return node; }

	private:

		wxPGProperty* property;
		int keyColumn;
		int valueColumn;
		Cornucopia::Node* node;
	};

	//=================================================================================
	wxDECLARE_EXPORTED_EVENT( CORNUCOPIA_PLUGIN_API, CORNUCOPIA_PLUGIN_PROPERTY_REFRESH, PluginPropertyEvent );

	//=================================================================================
	class CORNUCOPIA_PLUGIN_API PluginFrameEvent : public PluginEvent
	{
	public:

		PluginFrameEvent( int winid = 0, wxEventType commandType = wxEVT_NULL );
		virtual ~PluginFrameEvent( void );

		virtual wxEvent* Clone( void ) const override;

		void SetFrame( wxFrame* frame ) { this->frame = frame; }
		wxFrame* GetFrame( void ) { return frame; }

		void SetActivated( bool activated ) { this->activated = activated; }
		bool GetActivated( void ) { return activated; }

	private:

		wxFrame* frame;
		bool activated;
	};

	//=================================================================================
	wxDECLARE_EXPORTED_EVENT( CORNUCOPIA_PLUGIN_API, CORNUCOPIA_PLUGIN_FRAME_ACTIVATED, PluginFrameEvent );
	wxDECLARE_EXPORTED_EVENT( CORNUCOPIA_PLUGIN_API, CORNUCOPIA_PLUGIN_FRAME_GOT_FOCUS, PluginFrameEvent );
	wxDECLARE_EXPORTED_EVENT( CORNUCOPIA_PLUGIN_API, CORNUCOPIA_PLUGIN_FRAME_LOST_FOCUS, PluginFrameEvent );

	//=================================================================================
	class CORNUCOPIA_PLUGIN_API PluginFileEvent : public PluginEvent
	{
	public:

		PluginFileEvent( int winid = 0, wxEventType commandType = wxEVT_NULL );
		virtual ~PluginFileEvent( void );

		virtual wxEvent* Clone( void ) const override;

		void SetFile( const wxString& file ) { this->file = file; }
		const wxString& GetFile( void ) { return file; }

	private:

		wxString file;
	};

	//=================================================================================
	wxDECLARE_EXPORTED_EVENT( CORNUCOPIA_PLUGIN_API, CORNUCOPIA_PLUGIN_FILE_CREATED, PluginFileEvent );
	wxDECLARE_EXPORTED_EVENT( CORNUCOPIA_PLUGIN_API, CORNUCOPIA_PLUGIN_FILE_RESOLVE_NONWRITABLE, PluginFileEvent );

	//=================================================================================
	class CORNUCOPIA_PLUGIN_API PluginTweakEvent : public PluginEvent
	{
	public:

		PluginTweakEvent( int winid = 0, wxEventType commandType = wxEVT_NULL );
		virtual ~PluginTweakEvent( void );

		virtual wxEvent* Clone( void ) const override;

		void SetNode( Cornucopia::Node* node ) { this->node = node; }
		Cornucopia::Node* GetNode( void ) { return node; }

	private:

		Cornucopia::Node* node;
	};

	//=================================================================================
	wxDECLARE_EXPORTED_EVENT( CORNUCOPIA_PLUGIN_API, CORNUCOPIA_PLUGIN_TWEAK_NODE_CHANGED, PluginTweakEvent );
	wxDECLARE_EXPORTED_EVENT( CORNUCOPIA_PLUGIN_API, CORNUCOPIA_PLUGIN_TWEAK_NODE_SCRUBBING, PluginTweakEvent );
	wxDECLARE_EXPORTED_EVENT( CORNUCOPIA_PLUGIN_API, CORNUCOPIA_PLUGIN_TWEAK_FULL_UPDATE, PluginTweakEvent );

	//=================================================================================
	// A plugin for the Cornucopia Editor is implemented as a derivative of this class.
	class Plugin : public wxEvtHandler
	{
	public:

		Plugin( void ) {}
		virtual ~Plugin( void ) {}

		virtual bool Initialize( PluginAssistant* assistant ) = 0;
		virtual void Finalize( PluginAssistant* assistant ) = 0;

		struct Info
		{
			wxString pluginName;
			wxString pluginAuthor;
			wxString pluginVersion;
			wxString pluginDesc;
		};

		virtual void ProvideInfo( Info& pluginInfo ) = 0;
	};
}

#endif //__CORNUCOPIA_PLUGIN__

// Plugin.h