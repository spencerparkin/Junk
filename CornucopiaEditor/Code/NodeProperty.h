// NodeProperty.h

namespace CornucopiaEditor
{
	//=================================================================================
	// Derivatives of this class act as "glue" between Cornucopia nodes and wxWidgets properties.
	class NodeProperty : public wxObject
	{
	public:

		wxDECLARE_ABSTRACT_CLASS( NodeProperty );

		NodeProperty( void );
		virtual ~NodeProperty( void );

		virtual bool PushNodeValueToVariant( wxVariant& variant ) const = 0;
		virtual bool PullNodeValueFromVariant( const wxVariant& variant ) = 0;

		bool PushNodeValueToProperty( void );
		bool PullNodeValueFromProperty( void );

		void SetNode( Cornucopia::Node* node );
		Cornucopia::Node* GetNode( void );

		static wxPGProperty* Create( Cornucopia::Node* node );
		static NodeProperty* LameCast( wxPGProperty* property );

		void SetProperty( wxPGProperty* property );
		wxPGProperty* GetProperty( void );

	protected:

		Cornucopia::Node* node;
	};
}

// NodeProperty.h