// BoundedFloatPairPropertyEditor.h

namespace CornucopiaEditor
{
	//=================================================================================
	class BoundedFloatPairPropertyEditor : public CompanionedTextPropertyEditor
	{
	public:

		wxDECLARE_DYNAMIC_CLASS( BoundedFloatPairPropertyEditor );

	public:

		BoundedFloatPairPropertyEditor( void );
		virtual ~BoundedFloatPairPropertyEditor( void );

		virtual bool OnEvent( wxPropertyGrid* propertyGrid, wxPGProperty* property, wxWindow* control, wxEvent& event ) const override;
		virtual void DrawValue( wxDC& dc, const wxRect& rect, wxPGProperty* property, const wxString& text ) const;

	protected:

		virtual wxControl* CreateCompanionControl( wxWindow* parent ) const override;
		virtual bool IsCompanionControl( wxWindow* window ) const override;
		virtual bool IsCompanionControlValueChangedEvent( wxEvent& event ) const override;
		virtual bool IsCompanionControlScrubbing( wxEvent& event ) const override;

		Cornucopia::BoundedFloatPairNode* GetBoundedFloatPairNode( wxPGProperty* property ) const;

		virtual void UpdateCompanionControlUsingProperty( wxPGProperty* property, wxControl* control ) const override;
		virtual void UpdateTextCtrlUsingProperty( wxPGProperty* property, wxTextCtrl* textCtrl ) const override;

		virtual void UpdatePropertyUsingCompanionControl( wxPGProperty* property, wxControl* control ) const override;
		virtual void UpdatePropertyUsingTextCtrl( wxPGProperty* property, wxTextCtrl* textCtrl ) const override;

		wxString MakeValueText( wxPGProperty* property ) const;
	};
}

// BoundedFloatPairPropertyEditor.h