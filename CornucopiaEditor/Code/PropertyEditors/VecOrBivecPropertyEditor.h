// VecOrBivecPropertyEditor.h

namespace CornucopiaEditor
{
	//=================================================================================
	class VecOrBivecPropertyEditor : public wxPGEditor
	{
		wxDECLARE_DYNAMIC_CLASS( VecOrBivecPropertyEditor );

	public:

		VecOrBivecPropertyEditor( void );
		virtual ~VecOrBivecPropertyEditor( void );

		virtual wxPGWindowList CreateControls( wxPropertyGrid* propertyGrid, wxPGProperty* property, const wxPoint& pos, const wxSize& size ) const override;
		virtual void UpdateControl( wxPGProperty* property, wxWindow* control ) const override;
		virtual void DrawValue( wxDC& dc, const wxRect& rect, wxPGProperty* property, const wxString& text ) const override;
		virtual bool OnEvent( wxPropertyGrid* propertyGrid, wxPGProperty* property, wxWindow* control, wxEvent& event ) const override;
		virtual bool GetValueFromControl( wxVariant& variant, wxPGProperty* property, wxWindow* control ) const override;
		virtual void SetValueToUnspecified( wxPGProperty* property, wxWindow* control ) const override;
		virtual void OnFocus( wxPGProperty* property, wxWindow* window ) const override;

	private:

		void GrabVecOrBivecPointer( wxPGProperty* property, Cornucopia::VectorE3GANode*& vectorNode, Cornucopia::BivectorE3GANode*& bivectorNode ) const;

		enum
		{
			ID_Label0 = wxID_HIGHEST,
			ID_Label1,
			ID_Label2,
			ID_Component0,
			ID_Component1,
			ID_Component2,
		};
	};
}

// VecOrBivecPropertyEditor.h