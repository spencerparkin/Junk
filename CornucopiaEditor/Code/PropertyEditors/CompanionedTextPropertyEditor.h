// CompanionedTextPropertyEditor.h

namespace CornucopiaEditor
{
	//=================================================================================
	class CompanionedTextPropertyEditor : public wxPGEditor
	{
		wxDECLARE_DYNAMIC_CLASS( CompanionedTextPropertyEditor );

	public:

		CompanionedTextPropertyEditor( void );
		virtual ~CompanionedTextPropertyEditor( void );

		virtual wxPGWindowList CreateControls( wxPropertyGrid* propertyGrid, wxPGProperty* property, const wxPoint& pos, const wxSize& size ) const override;
		virtual void UpdateControl( wxPGProperty* property, wxWindow* control ) const override;
		virtual void DrawValue( wxDC& dc, const wxRect& rect, wxPGProperty* property, const wxString& text ) const override;
		virtual bool OnEvent( wxPropertyGrid* propertyGrid, wxPGProperty* property, wxWindow* control, wxEvent& event ) const override;
		virtual bool GetValueFromControl( wxVariant& variant, wxPGProperty* property, wxWindow* control ) const override;
		virtual void SetValueToUnspecified( wxPGProperty* property, wxWindow* control ) const override;
		virtual void OnFocus( wxPGProperty* property, wxWindow* window ) const override;

	protected:

		virtual wxControl* CreateCompanionControl( wxWindow* parent ) const = 0;
		virtual bool IsCompanionControl( wxWindow* window ) const = 0;
		virtual bool IsCompanionControlValueChangedEvent( wxEvent& event ) const = 0;
		virtual bool IsCompanionControlScrubbing( wxEvent& event ) const = 0;

		virtual void UpdateCompanionControlUsingProperty( wxPGProperty* property, wxControl* control ) const = 0;
		virtual void UpdateTextCtrlUsingProperty( wxPGProperty* property, wxTextCtrl* textCtrl ) const = 0;

		void UpdateProperty( wxPGProperty* property, wxWindow* control ) const;

		virtual void UpdatePropertyUsingCompanionControl( wxPGProperty* property, wxControl* control ) const = 0;
		virtual void UpdatePropertyUsingTextCtrl( wxPGProperty* property, wxTextCtrl* textCtrl ) const = 0;
	};
}

// CompanionedTextPropertyEditor.h