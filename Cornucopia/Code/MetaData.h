// MetaData.h

namespace Cornucopia
{
	class Context;
	class Node;

	//=================================================================================
	// Suppose an application that works with Cornucopia trees wants to tack
	// additional information on every node in the tree.  One way it can do
	// this is by creating a class derivative of every node-type.  These derived
	// types can then override the methods that read and write data between
	// nodes and Lua tables, as well as copy data between nodes.  All that would
	// remain is for the application to register it's own creator functions in
	// place of all other creator functions.  This would work, but requires the
	// user to create a lot of new classes!  By deriving off of the following
	// class, the meta-data class, the application is given a way that it can easily
	// provide its own meta-data on every node in a Cornucopia tree.  Since meta
	// data is application defined, an application that played nice with others
	// would preserve any foreign meta-data it encountered, though any given
	// application should not depend upon others to do such a thing.
	class CORNUCOPIA_API MetaData
	{
	public:

		MetaData( void );
		virtual ~MetaData( void );

		virtual bool ReadFromTable( lua_State* L, Context& context ) = 0;
		virtual bool WriteToTable( lua_State* L, Context& context ) const = 0;

		virtual bool Copy( const MetaData* metaData, Context& context ) = 0;

		virtual void PostCreate( Cornucopia::Node* node );
		virtual void PreDestroy( Cornucopia::Node* node );
	};
}

// MetaData.h