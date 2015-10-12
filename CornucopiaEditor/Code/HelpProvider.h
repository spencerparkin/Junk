// HelpProvider.h

namespace CornucopiaEditor
{
	//=================================================================================
	class HelpProvider : public wxHelpControllerHelpProvider
	{
	public:

		HelpProvider( void );
		virtual ~HelpProvider( void );

		virtual bool ShowHelpAtPoint( wxWindowBase* window, const wxPoint& point, wxHelpEvent::Origin origin ) override;
	};
}

// HelpProvider.h