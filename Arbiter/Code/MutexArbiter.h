// MutexArbiter.h

#pragma once

#include "Arbiter.h"
#include <wx/dataview.h>

WX_DECLARE_STRING_HASH_MAP( wxString, MutexPoolMap );

class MutexArbiter : public Arbiter
{
public:

	MutexArbiter( void );
	virtual ~MutexArbiter( void );

	virtual void AddPageToNoteBook( wxNotebook* noteBook ) override;
	virtual bool CanHandleRequestType( const wxString& requestType ) override;
	virtual void HandleRequest( const wxXmlNode* xmlRequestNode, wxXmlNode* xmlResponseNode, const wxString& clientName ) override;
	virtual void DroppedClient( const wxString& clientName ) override;
	virtual void UpdateView( void ) override;

private:

	MutexPoolMap mutexPoolMap;

	class DataViewModel : public wxDataViewModel
	{
	public:

		DataViewModel( MutexPoolMap* mutexPoolMap );
		virtual ~DataViewModel( void );

		virtual bool IsContainer( const wxDataViewItem& item ) const override;
		virtual wxDataViewItem GetParent( const wxDataViewItem& item ) const override;
		virtual unsigned int GetChildren( const wxDataViewItem& item, wxDataViewItemArray& children ) const override;
		virtual unsigned int GetColumnCount( void ) const override;
		virtual wxString GetColumnType( unsigned int col ) const override;
		virtual void GetValue( wxVariant& variant, const wxDataViewItem& item, unsigned int col ) const override;
		virtual bool SetValue( const wxVariant& variant, const wxDataViewItem& item, unsigned int col ) override;

		MutexPoolMap* mutexPoolMap;
		mutable wxArrayString keyArray;
	};

	wxDataViewCtrl* dataView;
};

// MutexArbiter.h
