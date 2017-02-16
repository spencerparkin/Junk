// Frame.h

#pragma once

#include <wx/frame.h>
#include <wx/timer.h>
#include <wx/dataview.h>
#include <wx/notebook.h>

class Frame : public wxFrame
{
public:

	Frame( void );
	virtual ~Frame( void );

	wxDataViewCtrl* GetClientDataView( void ) { return clientDataView; }

private:

	enum
	{
		ID_Exit = wxID_HIGHEST,
		ID_Timer,
	};

	wxTimer timer;
	bool inTimer;

	void OnTimer( wxTimerEvent& event );
	void OnExit( wxCommandEvent& event );

	class ClientDataViewModel : public wxDataViewModel
	{
	public:

		ClientDataViewModel( void );
		virtual ~ClientDataViewModel( void );

		virtual bool IsContainer( const wxDataViewItem& item ) const override;
		virtual wxDataViewItem GetParent( const wxDataViewItem& item ) const override;
		virtual unsigned int GetChildren( const wxDataViewItem& item, wxDataViewItemArray& children ) const override;
		virtual unsigned int GetColumnCount( void ) const override;
		virtual wxString GetColumnType( unsigned int col ) const override;
		virtual void GetValue( wxVariant& variant, const wxDataViewItem& item, unsigned int col ) const override;
		virtual bool SetValue( const wxVariant& variant, const wxDataViewItem& item, unsigned int col ) override;
	};

	wxNotebook* noteBook;
	wxDataViewCtrl* clientDataView;
};

// Frame.h