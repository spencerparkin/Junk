// VecOrBivecProperty.h

namespace CornucopiaEditor
{
	//=================================================================================
	// Let the user edit the components of the vector or bivector.
	class VecOrBivecProperty : public wxPGProperty, public NodeProperty
	{
		wxDECLARE_DYNAMIC_CLASS( VecOrBivecProperty );

	public:

		static VecOrBivecProperty* Create( Cornucopia::Node* node );

		VecOrBivecProperty( const wxString& label = wxPG_LABEL, const wxString& name = wxPG_LABEL );
		virtual ~VecOrBivecProperty( void );

		virtual bool ValidateValue( wxVariant& value, wxPGValidationInfo& validationInfo ) const override;

		const wxPGEditor* DoGetEditorClass( void ) const override;

		virtual bool PushNodeValueToVariant( wxVariant& variant ) const override;
		virtual bool PullNodeValueFromVariant( const wxVariant& variant ) override;

	private:

		static VecOrBivecPropertyEditor* editor;
	};
}

// VecOrBivecProperty.h