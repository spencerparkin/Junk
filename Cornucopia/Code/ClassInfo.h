// ClassInfo.h

namespace Cornucopia
{
	//=================================================================================
	class CORNUCOPIA_API ClassInfo
	{
	public:

		ClassInfo( const std::string& type, ClassInfo* baseClassInfo0 = 0, ClassInfo* baseClassInfo1 = 0, ClassInfo* baseClassInfo2 = 0 )
		{
			this->type = type;

			for( int index = 0; index < MAX_BASE_CLASSES; index++ )
				baseClassInfoList[ index ] = 0;

			baseClassInfoList[0] = baseClassInfo0;
			baseClassInfoList[1] = baseClassInfo1;
			baseClassInfoList[2] = baseClassInfo2;
		}
		
		~ClassInfo( void )
		{
		}

		const std::string& Type( void ) const
		{
			return type;
		}

		bool IsOrInheritsFromClass( ClassInfo* classInfo )
		{
			return IsOrInheritsFromClass( classInfo->type );
		}

		bool IsOrInheritsFromClass( const std::string& type )
		{
			if( this->type == type )
				return true;

			for( int index = 0; index < MAX_BASE_CLASSES; index++ )
			{
				ClassInfo* baseClassInfo = baseClassInfoList[ index ];
				if( baseClassInfo && baseClassInfo->IsOrInheritsFromClass( type ) )
					return true;
			}

			return false;
		}

	private:

		std::string type;
		enum { MAX_BASE_CLASSES = 3 };
		ClassInfo* baseClassInfoList[ MAX_BASE_CLASSES ];
	};
}

//=================================================================================
#define DECL_CORNUCOPIA_CLASSINFO( ClassName ) \
	public: \
	virtual ClassInfo* ThisClassInfo( void ) const; \
	static ClassInfo* RetrieveClassInfo( void ); \
	template< typename ClassType > \
	ClassType* Cast( void ) \
	{ \
		ClassInfo* thisClassInfo = ThisClassInfo(); \
		if( thisClassInfo->IsOrInheritsFromClass( ClassType::RetrieveClassInfo() ) ) \
			return ( ClassType* )this; \
		return 0; \
	} \
	template< typename ClassType > \
	const ClassType* Cast( void ) const \
	{ \
		return const_cast< ClassName* >( this )->Cast< ClassType >(); \
	} \
	static ClassName* Create( void ); \
	static ClassInfo classInfo

//=================================================================================
#define IMPL_CORNUCOPIA_CLASSINFO( ClassName ) \
	/*virtual*/ ClassInfo* ClassName::ThisClassInfo( void ) const \
	{ \
		return &ClassName::classInfo; \
	} \
	/*static*/ ClassInfo* ClassName::RetrieveClassInfo( void ) \
	{ \
		return &classInfo; \
	} \
	/*static*/ ClassName* ClassName::Create( void ) \
	{ \
		return new ClassName(); \
	}

//=================================================================================
#define IMPL_CORNUCOPIA_CLASSINFO0( ClassName ) \
	IMPL_CORNUCOPIA_CLASSINFO( ClassName ) \
	ClassInfo ClassName##::classInfo( #ClassName )

//=================================================================================
#define IMPL_CORNUCOPIA_CLASSINFO1( ClassName, BaseClassName1 ) \
	IMPL_CORNUCOPIA_CLASSINFO( ClassName ) \
	ClassInfo ClassName##::classInfo( #ClassName, &BaseClassName1::classInfo )

//=================================================================================
#define IMPL_CORNUCOPIA_CLASSINFO2( ClassName, BaseClassName1, BaseClassName2 ) \
	IMPL_CORNUCOPIA_CLASSINFO( ClassName ) \
	ClassInfo ClassName##::classInfo( #ClassName, &BaseClassName1::classInfo, &BaseClassName2::classInfo )

//=================================================================================
#define IMPL_CORNUCOPIA_CLASSINFO3( ClassName, BaseClassName1, BaseClassName2, BaseClassName3 ) \
	IMPL_CORNUCOPIA_CLASSINFO( ClassName ) \
	ClassInfo ClassName##::classInfo( #ClassName, &BaseClassName1::classInfo, &BaseClassName2::classInfo, &BaseClassName3::classInfo )

// ClassInfo.h