// Main.cpp

#include "Cornucopia.h"

using namespace Cornucopia;

class TestMetaData : public Cornucopia::MetaData
{
public:

	static Cornucopia::MetaData* Create( void )
	{
		return new TestMetaData();
	}

	TestMetaData( void ) {}
	virtual ~TestMetaData( void ) {}

	virtual bool ReadFromTable( lua_State* L, Context& context ) override
	{
		lua_getfield( L, -1, "testKey" );
		value = lua_tostring( L, -1 );
		lua_pop( L, 1 );
		return true;
	}

	virtual bool WriteToTable( lua_State* L, Context& context ) const override
	{
		lua_pushstring( L, "testValue" );
		lua_setfield( L, -2, "testKey" );
		return true;
	}

	virtual bool Copy( const MetaData* metaData, Context& context )
	{
		const TestMetaData* testMetaData = ( const TestMetaData* )metaData;
		value = testMetaData->value;
		return true;
	}

	std::string value;
};

int main( int argc, char** argv )
{
	Context context;

	context.SetMetaDataCreatorFunc( TestMetaData::Create );

	if( !context.RegisterAllBuiltinNodeCreators() )
		context.DumpAllErrors( stderr );
	else
	{
		Node* rootNode = Node::CreateAndReadFromFile( "../Code/TestProgram/TestFile.lua", context );
		if( !rootNode )
			context.DumpAllErrors( stderr );
		else
		{
			if( !rootNode->WriteToFile( "../Code/TestProgram/TestFile2.lua", context ) )
				context.DumpAllErrors( stderr );

			delete rootNode;
		}
	}

	return 0;
}

// Main.cpp