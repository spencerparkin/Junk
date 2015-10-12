// ConsoleOutputInterface.h

#ifndef __ConsoleOutputInterface_h__
#define __ConsoleOutputInterface_h__

#include <wx/richtext/richtextctrl.h>
#include <wx/sizer.h>

#include "Interface.h"

namespace Geometer
{
	class ConsoleOutputInterface;
}

class Geometer::ConsoleOutputInterface : public Geometer::Interface
{
public:

	wxDECLARE_DYNAMIC_CLASS( ConsoleOutputInterface );

	ConsoleOutputInterface( void );
	virtual ~ConsoleOutputInterface( void );

	virtual void FillOutRegistryEntry( RegistryEntry& entry ) override;
	virtual bool CreateControls( void ) override;
	virtual bool UpdateControls( void ) override;

private:

	void Print( const wxString& text );

	bool GenerateLatex( const wxString& latexCode, wxImage& latexImage );

	wxString latexBinDirPath;
	wxString latexWorkDirPath;

	int entriesRead;

	wxRichTextCtrl* textControl;
};

#endif //__ConsoleOutputInterface_h__

// ConsoleOutputInterface.h