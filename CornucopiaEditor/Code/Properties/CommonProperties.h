// CommonProperties.h

namespace CornucopiaEditor
{
	//=================================================================================
	class DefaultProperty : public wxStringProperty, public NodeProperty
	{
		wxDECLARE_DYNAMIC_CLASS( DefaultProperty );

	public:

		static DefaultProperty* Create( Cornucopia::Node* node );

		DefaultProperty( const wxString& label = wxPG_LABEL, const wxString& name = wxPG_LABEL );
		virtual ~DefaultProperty( void );

		virtual wxString ValueToString( wxVariant& variant, int argFlags = 0 ) const override;
		virtual bool StringToValue( wxVariant& variant, const wxString& text, int argFlags = 0 ) const override;

		virtual bool ValidateValue( wxVariant& value, wxPGValidationInfo& validationInfo ) const override;

		virtual bool PushNodeValueToVariant( wxVariant& variant ) const override;
		virtual bool PullNodeValueFromVariant( const wxVariant& variant ) override;
	};

	//=================================================================================
	class StringProperty : public wxStringProperty, public NodeProperty
	{
		wxDECLARE_DYNAMIC_CLASS( StringProperty );

	public:

		static StringProperty* Create( Cornucopia::Node* node );

		StringProperty( const wxString& label = wxPG_LABEL, const wxString& name = wxPG_LABEL, const wxString& value = wxEmptyString );
		virtual ~StringProperty( void );

		virtual bool PushNodeValueToVariant( wxVariant& variant ) const override;
		virtual bool PullNodeValueFromVariant( const wxVariant& variant ) override;
	};

	//=================================================================================
	class IntProperty : public wxIntProperty, public NodeProperty
	{
		wxDECLARE_DYNAMIC_CLASS( IntProperty );

	public:

		static IntProperty* Create( Cornucopia::Node* node );

		IntProperty( const wxString& label = wxPG_LABEL, const wxString& name = wxPG_LABEL, long value = 0 );
		virtual ~IntProperty( void );

		virtual bool PushNodeValueToVariant( wxVariant& variant ) const override;
		virtual bool PullNodeValueFromVariant( const wxVariant& variant ) override;
	};

	//=================================================================================
	class FloatProperty : public wxFloatProperty, public NodeProperty
	{
		wxDECLARE_DYNAMIC_CLASS( FloatProperty );

	public:

		static FloatProperty* Create( Cornucopia::Node* node );

		FloatProperty( const wxString& label = wxPG_LABEL, const wxString& name = wxPG_LABEL, double value = 0.0 );
		virtual ~FloatProperty( void );

		virtual bool PushNodeValueToVariant( wxVariant& variant ) const override;
		virtual bool PullNodeValueFromVariant( const wxVariant& variant ) override;

		virtual bool ValidateValue( wxVariant& value, wxPGValidationInfo& validationInfo ) const override;
	};

	//=================================================================================
	class BoolProperty : public wxBoolProperty, public NodeProperty
	{
		wxDECLARE_DYNAMIC_CLASS( BoolProperty );

	public:

		static BoolProperty* Create( Cornucopia::Node* node );

		BoolProperty( const wxString& label = wxPG_LABEL, const wxString& name = wxPG_LABEL, bool value = false );
		virtual ~BoolProperty( void );

		virtual bool PushNodeValueToVariant( wxVariant& variant ) const override;
		virtual bool PullNodeValueFromVariant( const wxVariant& variant ) override;
	};

	//=================================================================================
	class MultiChoiceProperty : public wxMultiChoiceProperty, public NodeProperty
	{
		wxDECLARE_DYNAMIC_CLASS( MultiChoiceProperty );

	public:

		static MultiChoiceProperty* Create( Cornucopia::Node* node );

		MultiChoiceProperty( const wxString& label, const wxString& name, const wxPGChoices& choices );
		virtual ~MultiChoiceProperty( void );

		virtual bool PushNodeValueToVariant( wxVariant& variant ) const override;
		virtual bool PullNodeValueFromVariant( const wxVariant& variant ) override;

		virtual bool ValidateValue( wxVariant& value, wxPGValidationInfo& validationInfo ) const override;
	};

	//=================================================================================
	class FileProperty : public wxFileProperty, public NodeProperty
	{
		wxDECLARE_DYNAMIC_CLASS( FileProperty );

	public:

		static FileProperty* Create( Cornucopia::Node* node );

		FileProperty( const wxString& label = wxPG_LABEL, const wxString& name = wxPG_LABEL, const wxString& value = wxEmptyString );
		virtual ~FileProperty( void );

		virtual bool PushNodeValueToVariant( wxVariant& variant ) const override;
		virtual bool PullNodeValueFromVariant( const wxVariant& variant ) override;
	};

	//=================================================================================
	class ColorProperty : public wxColourProperty, public NodeProperty
	{
	public:

		wxDECLARE_DYNAMIC_CLASS( ColorProperty );

	public:

		static ColorProperty* Create( Cornucopia::Node* node );

		ColorProperty( const wxString& label = wxPG_LABEL, const wxString& name = wxPG_LABEL, const wxColour& value = *wxWHITE );
		virtual ~ColorProperty( void );

		virtual bool PushNodeValueToVariant( wxVariant& variant ) const override;
		virtual bool PullNodeValueFromVariant( const wxVariant& variant ) override;
	};
}

// CommonProperties.h