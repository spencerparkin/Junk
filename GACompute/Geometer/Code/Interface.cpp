// Interface.cpp

#include "Interface.h"

namespace Geometer
{
	wxIMPLEMENT_DYNAMIC_CLASS( Interface, wxPanel );
}

using namespace Geometer;

Interface::Registry Interface::registry;

int Interface::masterUpdateKey = 0;

/*static*/ void Interface::PopulateRegistry( void )
{
	wxClassInfo* interfaceClassInfo = wxClassInfo::FindClass( "Interface" );
	if( !interfaceClassInfo )
		return;

	const wxClassInfo* classInfo = wxClassInfo::GetFirst();
	while( classInfo )
	{
		if( classInfo != interfaceClassInfo && classInfo->IsKindOf( interfaceClassInfo ) )
			Register( classInfo );

		classInfo = classInfo->GetNext();
	}
}

/*static*/ bool Interface::Register( const wxClassInfo* classInfo )
{
	wxString className = classInfo->GetClassName();
	if( registry.find( className ) != registry.end() )
		return false;

	// No interface should do any significant amount of processing
	// in its constructor; so creating a temporary interface only
	// to probe it for information before deleting it should be fine.
	wxObjectConstructorFn constructorFunc = classInfo->GetConstructor();
	wxObject* object = constructorFunc();
	Interface* iface = wxDynamicCast( object, Interface );
	wxASSERT( iface != 0 );
	RegistryEntry entry;
	entry.classInfo = classInfo;
	entry.interfaceMenuItemId = -1;
	iface->FillOutRegistryEntry( entry );
	delete object;

	registry[ className ] = entry;
	return true;
}

/*static*/ bool Interface::Unregister( const wxClassInfo* classInfo )
{
	wxString className = classInfo->GetClassName();
	Registry::iterator iter = registry.find( className );
	if( iter == registry.end() )
		return false;

	registry.erase( iter );
	return true;
}

/*static*/ const Interface::Registry& Interface::GetRegistry( void )
{
	return registry;
}

Interface::Interface( void )
{
	updateKey = masterUpdateKey - 1;
}

/*virtual*/ Interface::~Interface( void )
{
}

void Interface::TouchedLuaState( bool updateThisToo /*= true*/ )
{
	masterUpdateKey++;
	
	if( !updateThisToo )
		updateKey = masterUpdateKey;
}

// Interface.cpp