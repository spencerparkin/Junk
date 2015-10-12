// Plugin.cpp

#include "Plugin.h"

namespace CornucopiaEditor
{
	//=================================================================================
	wxDEFINE_EVENT( CORNUCOPIA_PLUGIN_INITIALIZE, PluginEvent );
	wxDEFINE_EVENT( CORNUCOPIA_PLUGIN_FINALIZE, PluginEvent );

	//=================================================================================
	wxDEFINE_EVENT( CORNUCOPIA_PLUGIN_PAGE_CREATED, PluginPageEvent );
	wxDEFINE_EVENT( CORNUCOPIA_PLUGIN_PAGE_CHANGED, PluginPageEvent );
	wxDEFINE_EVENT( CORNUCOPIA_PLUGIN_PAGE_DESTROYING, PluginPageEvent );
	wxDEFINE_EVENT( CORNUCOPIA_PLUGIN_PAGE_AUGMENT_UI, PluginPageEvent );

	//=================================================================================
	wxDEFINE_EVENT( CORNUCOPIA_PLUGIN_DATAVIEWCTRL_GET_VALUE, PluginDataViewCtrlEvent );
	wxDEFINE_EVENT( CORNUCOPIA_PLUGIN_DATAVIEWCTRL_SET_VALUE, PluginDataViewCtrlEvent );

	//=================================================================================
	wxDEFINE_EVENT( CORNUCOPIA_PLUGIN_PROPERTY_REFRESH, PluginPropertyEvent );

	//=================================================================================
	wxDEFINE_EVENT( CORNUCOPIA_PLUGIN_FRAME_ACTIVATED, PluginFrameEvent );
	wxDEFINE_EVENT( CORNUCOPIA_PLUGIN_FRAME_GOT_FOCUS, PluginFrameEvent );
	wxDEFINE_EVENT( CORNUCOPIA_PLUGIN_FRAME_LOST_FOCUS, PluginFrameEvent );

	//=================================================================================
	wxDEFINE_EVENT( CORNUCOPIA_PLUGIN_FILE_CREATED, PluginFileEvent );
	wxDEFINE_EVENT( CORNUCOPIA_PLUGIN_FILE_RESOLVE_NONWRITABLE, PluginFileEvent );

	//=================================================================================
	wxDEFINE_EVENT( CORNUCOPIA_PLUGIN_TWEAK_NODE_CHANGED, PluginTweakEvent );
	wxDEFINE_EVENT( CORNUCOPIA_PLUGIN_TWEAK_NODE_SCRUBBING, PluginTweakEvent );
	wxDEFINE_EVENT( CORNUCOPIA_PLUGIN_TWEAK_FULL_UPDATE, PluginTweakEvent );
}

using namespace CornucopiaEditor;

//=================================================================================
PluginEvent::PluginEvent( int winid /*= 0*/, wxEventType commandType /*= wxEVT_NULL*/ ) : wxEvent( winid, commandType )
{
	assistant = 0;
}

//=================================================================================
/*virtual*/ PluginEvent::~PluginEvent( void )
{
}

//=================================================================================
/*virtual*/ wxEvent* PluginEvent::Clone( void ) const
{
	PluginEvent* event = new PluginEvent( GetId(), GetEventType() );
	event->SetAssistant( assistant );
	event->SetLibraryPath( libraryPath );
	return event;
}

//=================================================================================
PluginPageEvent::PluginPageEvent( int winid /*= 0*/, wxEventType commandType /*= wxEVT_NULL*/ ) : PluginEvent( winid, commandType )
{
	page = 0;
}

//=================================================================================
/*virtual*/ PluginPageEvent::~PluginPageEvent( void )
{
}

//=================================================================================
/*virtual*/ wxEvent* PluginPageEvent::Clone() const
{
	PluginPageEvent* event = new PluginPageEvent( GetId(), GetEventType() );
	event->SetAssistant( assistant );
	event->SetLibraryPath( libraryPath );
	event->SetPage( page );
	return event;
}

//=================================================================================
PluginDataViewCtrlEvent::PluginDataViewCtrlEvent( int winid /*= 0*/, wxEventType commandType /*= wxEVT_NULL*/ ) : PluginEvent( winid, commandType )
{
	dataViewCtrl = 0;
	col = -1;
}

//=================================================================================
/*virtual*/ PluginDataViewCtrlEvent::~PluginDataViewCtrlEvent( void )
{
}

//=================================================================================
/*virtual*/ wxEvent* PluginDataViewCtrlEvent::Clone() const
{
	PluginDataViewCtrlEvent* event = new PluginDataViewCtrlEvent( GetId(), GetEventType() );
	event->SetAssistant( assistant );
	event->SetLibraryPath( libraryPath );
	event->SetDataViewCtrl( dataViewCtrl );
	event->SetDataViewItem( dataViewItem );
	event->SetVariant( variant );
	event->SetColumn( col );
	return event;
}

//=================================================================================
PluginPropertyEvent::PluginPropertyEvent( int winid /*= 0*/, wxEventType commandType /*= wxEVT_NULL*/ ) : PluginEvent( winid, commandType )
{
	property = 0;
	keyColumn = -1;
	valueColumn = -1;
	node = 0;
}

//=================================================================================
/*virtual*/ PluginPropertyEvent::~PluginPropertyEvent( void )
{
}

//=================================================================================
/*virtual*/ wxEvent* PluginPropertyEvent::Clone() const
{
	PluginPropertyEvent* event = new PluginPropertyEvent();
	event->SetAssistant( assistant );
	event->SetLibraryPath( libraryPath );
	event->SetProperty( property );
	event->SetKeyColumn( keyColumn );
	event->SetValueColumn( valueColumn );
	event->SetNode( node );
	return event;
}

//=================================================================================
PluginFrameEvent::PluginFrameEvent( int winid /*= 0*/, wxEventType commandType /*= wxEVT_NULL*/ ) : PluginEvent( winid, commandType )
{
}

//=================================================================================
/*virtual*/ PluginFrameEvent::~PluginFrameEvent( void )
{
}

//=================================================================================
/*virtual*/ wxEvent* PluginFrameEvent::Clone() const
{
	PluginFrameEvent* event = new PluginFrameEvent( GetId(), GetEventType() );
	event->SetAssistant( assistant );
	event->SetLibraryPath( libraryPath );
	event->SetFrame( frame );
	return event;
}

//=================================================================================
PluginFileEvent::PluginFileEvent( int winid /*= 0*/, wxEventType commandType /*= wxEVT_NULL*/ ) : PluginEvent( winid, commandType )
{
}

//=================================================================================
/*virtual*/ PluginFileEvent::~PluginFileEvent( void )
{
}

//=================================================================================
/*virtual*/ wxEvent* PluginFileEvent::Clone() const
{
	PluginFileEvent* event = new PluginFileEvent( GetId(), GetEventType() );
	event->SetAssistant( assistant );
	event->SetLibraryPath( libraryPath );
	event->SetFile( file );
	return event;
}

//=================================================================================
PluginTweakEvent::PluginTweakEvent( int winid /*= 0*/, wxEventType commandType /*= wxEVT_NULL*/ ) : PluginEvent( winid, commandType )
{
}

//=================================================================================
/*virtual*/ PluginTweakEvent::~PluginTweakEvent( void )
{
}

//=================================================================================
/*virtual*/ wxEvent* PluginTweakEvent::Clone( void ) const
{
	PluginTweakEvent* event = new PluginTweakEvent( GetId(), GetEventType() );
	event->SetAssistant( assistant );
	event->SetLibraryPath( libraryPath );
	event->SetNode( node );
	return event;
}

// Plugin.cpp