// ModuleInterface.h

#ifndef __ModuleInterface_h__
#define __ModuleInterface_h__

#include <wx/sizer.h>
#include <wx/dataview.h>
#include <map>

#include "Interface.h"

namespace Geometer
{
	class ModuleInterface;
}

class Geometer::ModuleInterface : public Geometer::Interface
{
public:

	wxDECLARE_DYNAMIC_CLASS( ModuleInterface );

	ModuleInterface( void );
	virtual ~ModuleInterface( void );

	virtual void FillOutRegistryEntry( RegistryEntry& entry ) override;
	virtual bool CreateControls( void ) override;
	virtual bool UpdateControls( void ) override;

private:

	void OnContextMenu( wxDataViewEvent& event );

	class DataViewModel : public wxDataViewModel
	{
	public:

		DataViewModel( void );
		virtual ~DataViewModel( void );

		virtual unsigned int GetColumnCount() const override;
		virtual wxString GetColumnType( unsigned int col ) const override;
		virtual void GetValue( wxVariant& variant, const wxDataViewItem& item, unsigned int col ) const override;
		virtual bool SetValue( const wxVariant& variant, const wxDataViewItem& item, unsigned int col ) override;
		virtual wxDataViewItem GetParent( const wxDataViewItem& item ) const override;
	    virtual bool IsContainer( const wxDataViewItem& item ) const override;
		virtual unsigned int GetChildren( const wxDataViewItem& item, wxDataViewItemArray& children ) const override;

		struct Entry
		{
			enum Type { MODULE, API_CALL };
			Type type;
			wxString label;
			wxString description;
			wxString module;
		};

		typedef std::map< int, Entry > ItemMap;
		mutable ItemMap itemMap;
		mutable int nextId;

		bool LookupEntry( const wxDataViewItem& item, Entry& entry ) const;
		void MakeNewEntry( wxDataViewItem& item, const Entry& entry ) const;
	};

	wxDataViewCtrl* dataViewCtrl;

	wxDataViewItem contextItem;

	enum
	{
		ID_ContextMenu_LoadNewModule,
		ID_ContextMenu_UnloadThisModule,
		ID_ContextMenu_ReloadThisModule,
	};

	void OnContextMenu_LoadNewModule( wxCommandEvent& event );
	void OnContextMenu_UnloadThisModule( wxCommandEvent& event );
	void OnContextMenu_ReloadThisModule( wxCommandEvent& event );
};

#endif //__ModuleInterface_h__

// ModuleInterface.h