// BoundedFloatProperty.h

namespace CornucopiaEditor
{
	//=================================================================================
	class BoundedFloatProperty : public wxPGProperty, public NodeProperty
	{
		wxDECLARE_DYNAMIC_CLASS( BoundedFloatProperty );

	public:

		static BoundedFloatProperty* Create( Cornucopia::Node* node );

		BoundedFloatProperty( const wxString& label = wxPG_LABEL, const wxString& name = wxPG_LABEL );
		virtual ~BoundedFloatProperty( void );

		virtual bool ValidateValue( wxVariant& value, wxPGValidationInfo& validationInfo ) const override;

		virtual const wxPGEditor* DoGetEditorClass( void ) const override;

		virtual wxString ValueToString( wxVariant& variant, int argFlags = 0 ) const override;

		virtual bool PushNodeValueToVariant( wxVariant& variant ) const override;
		virtual bool PullNodeValueFromVariant( const wxVariant& variant ) override;

	private:

		static BoundedFloatPropertyEditor* editor;
	};
}

// BoundedFloatProperty.h