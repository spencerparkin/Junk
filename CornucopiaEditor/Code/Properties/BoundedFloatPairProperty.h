// BoundedFloatPairProperty.h

namespace CornucopiaEditor
{
	//=================================================================================
	class BoundedFloatPairProperty : public wxPGProperty, public NodeProperty
	{
		wxDECLARE_DYNAMIC_CLASS( BoundedFloatPairProperty );

	public:

		static BoundedFloatPairProperty* Create( Cornucopia::Node* node );

		BoundedFloatPairProperty( const wxString& label = wxPG_LABEL, const wxString& name = wxPG_LABEL );
		virtual ~BoundedFloatPairProperty( void );

		virtual bool ValidateValue( wxVariant& value, wxPGValidationInfo& validationInfo ) const override;

		virtual const wxPGEditor* DoGetEditorClass( void ) const override;

		virtual wxString ValueToString( wxVariant& variant, int argFlags = 0 ) const override;

		virtual bool PushNodeValueToVariant( wxVariant& variant ) const override;
		virtual bool PullNodeValueFromVariant( const wxVariant& variant ) override;

	private:

		static BoundedFloatPairPropertyEditor* editor;
	};
}

// BoundedFloatPairProperty.h