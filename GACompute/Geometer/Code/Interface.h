// Interface.h

#ifndef __Interface_h__
#define __Interface_h__

#include <wx/panel.h>
#include <wx/hashmap.h>

namespace Geometer
{
	class Interface;
}

// STPTODO: Provide an interface that lets the user load and unload Lua modules.  Have that interface persist state across instantiations of geometer?
// STPTODO: Provide an interface to all global variables?

// Derivatives of this class provide a UI into the Lua state.
// A derivative of this class may one day be created that allows
// plugins to extend the application by providing their own interfaces.
class Geometer::Interface : public wxPanel
{
public:

	struct RegistryEntry
	{
		const wxClassInfo* classInfo;
		wxString interfaceMenuItemName;
		wxString interfaceMenuItemHelp;
		mutable int interfaceMenuItemId;
	};

	WX_DECLARE_STRING_HASH_MAP( RegistryEntry, Registry );

	static void PopulateRegistry( void );
	static const Registry& GetRegistry( void );
	static bool Register( const wxClassInfo* classInfo );
	static bool Unregister( const wxClassInfo* classInfo );

	wxDECLARE_DYNAMIC_CLASS( Interface );

	Interface( void );
	virtual ~Interface( void );

	virtual void FillOutRegistryEntry( RegistryEntry& entry ) {}
	virtual bool CreateControls( void ) { return false; }	// Create UI into Lua state.
	virtual bool UpdateControls( void ) { return false; }	// Have UI reflect the Lua state.

	int GetUpdateKey( void ) { return updateKey; }
	void SetUpdateKey( int updateKey ) { this->updateKey = updateKey; }

	static int GetMasterUpdateKey( void ) { return masterUpdateKey; }

protected:

	// If an interfaces changes the Lua state in any way, it should
	// call this to cause control updates in all other interfaces.
	void TouchedLuaState( bool updateThisToo = true );

private:

	int updateKey;
	static int masterUpdateKey;

	static Registry registry;
};

#endif //__Interface_h__

// Interface.h