// BoundedPointListNode.cpp

#include "../Cornucopia.h"

using namespace Cornucopia;

IMPL_CORNUCOPIA_CLASSINFO1( BoundedPointListNode, Node );

//=================================================================================
BoundedPointListNode::BoundedPointListNode( void )
{
	pointList = new List();

	bounded = false;
	bounds.min.set( c3ga::vectorE2GA::coord_e1_e2, 0.0, 0.0 );
	bounds.max.set( c3ga::vectorE2GA::coord_e1_e2, 0.0, 0.0 );
}

//=================================================================================
/*virtual*/ BoundedPointListNode::~BoundedPointListNode( void )
{
	delete pointList;
}

//=================================================================================
/*virtual*/ bool BoundedPointListNode::ReadFromTable( lua_State* L, Context& context )
{
	if( !Node::ReadFromTable( L, context ) )
		return context.IssueError( "BoundedPointListNode failed to read base-class portion." );

	if( !Node::ReadBoolean( L, context, "bounded", bounded, false ) )
		return context.IssueError( "BoundedPointListNode \"%s\" failed to read \"bounded\".", GetName().c_str() );

	c3ga::vectorE2GA defaultBound;
	defaultBound.set( c3ga::vectorE2GA::coord_e1_e2, 0.0, 0.0 );
	if( !Node::ReadVectorE2GA( L, context, "minBounds", bounds.min, &defaultBound ) )
		return context.IssueError( "BoundedPointListNode \"%s\" failed to read \"minBounds\".", GetName().c_str() );

	defaultBound = bounds.min;
	if( !Node::ReadVectorE2GA( L, context, "maxBounds", bounds.max, &defaultBound ) )
		return context.IssueError( "BoundedPointListNode \"%s\" failed to read \"maxBounds\".", GetName().c_str() );

	int pointCount = 0;
	if( !Node::ReadInteger( L, context, "pointCount", pointCount, pointCount ) )
		return context.IssueError( "BoundedPointListNode \"%s\" failed to read \"pointCount\".", GetName().c_str() );

	pointList->clear();
	char formattedKey[ 32 ];
	for( int index = 0; index < pointCount; index++ )
	{
		sprintf_s( formattedKey, sizeof( formattedKey ), "point%d", index );
		c3ga::vectorE2GA point = c3ga::Lerp( bounds.min, bounds.max, 0.5 );
		if( !Node::ReadVectorE2GA( L, context, formattedKey, point, &point ) )
			return context.IssueError( "BoundedPointListNode \"%s\" failed to read \"%s\".", GetName().c_str(), formattedKey );
		pointList->push_back( point );
	}

	return true;
}

//=================================================================================
/*virtual*/ bool BoundedPointListNode::WriteToTable( lua_State* L, Context& context ) const
{
	if( !Node::WriteToTable( L, context ) )
		return context.IssueError( "BoundedPointListNode \"%s\" failed to write base-class portion.", GetName().c_str() );

	if( !Node::WriteBoolean( L, context, "bounded", bounded ) )
		return context.IssueError( "BoundedPointListNode \"%s\" failed to write \"bounded\".", GetName().c_str() );

	if( !Node::WriteVectorE2GA( L, context, "minBounds", bounds.min ) )
		return context.IssueError( "BoundedPointListNode \"%s\" failed to write \"minBounds\".", GetName().c_str() );

	if( !Node::WriteVectorE2GA( L, context, "maxBounds", bounds.max ) )
		return context.IssueError( "BoundedPointListNode \"%s\" failed to write \"maxBounds\".", GetName().c_str() );

	if( !Node::WriteInteger( L, context, "pointCount", pointList->size() ) )
		return context.IssueError( "BoundedPointListNode \"%s\" failed to write \"pointCount\".", GetName().c_str() );

	int index = 0;
	char formattedKey[ 32 ];
	for( List::const_iterator iter = pointList->begin(); iter != pointList->end(); iter++ )
	{
		sprintf_s( formattedKey, sizeof( formattedKey ), "point%d", index++ );
		c3ga::vectorE2GA point = *iter;
		if( !Node::WriteVectorE2GA( L, context, formattedKey, point ) )
			return context.IssueError( "BoundedPointListNode \"%s\" failed to write \"%s\".", GetName().c_str(), formattedKey );
	}

	return true;
}

//=================================================================================
/*virtual*/ bool BoundedPointListNode::Copy( const Node* node, Context& context, const CopyParameters& copyParameters )
{
	if( !Node::Copy( node, context, copyParameters ) )
		return context.IssueError( "BoundedPointListNode \"%s\" failed to copy base-class portion.", GetName().c_str() );

	const BoundedPointListNode* boundedPointListNode = ( const BoundedPointListNode* )node;
	bounded = boundedPointListNode->bounded;
	bounds = boundedPointListNode->bounds;
	*pointList = *boundedPointListNode->pointList;
	return true;
}

//=================================================================================
/*virtual*/ std::string BoundedPointListNode::GetValueToDisplayString( void ) const
{
	char formattedString[ 128 ];
	sprintf_s( formattedString, sizeof( formattedString ), "List size: %d; Bounded: %s, Min: [ %1.2f %1.2f ]; Max: [ %1.2f %1.2f ]",
							pointList->size(), ( bounded ? "Yes" : "No" ),
							bounds.min.get_e1(), bounds.min.get_e2(),
							bounds.max.get_e1(), bounds.max.get_e2() );
	return formattedString;
}

//=================================================================================
/*virtual*/ Node::ValueStringError BoundedPointListNode::GetValueToString( std::string& valueString ) const
{
	VarMap varMap;

	varMap[ "xMin" ] = bounds.min.get_e1();
	varMap[ "xMax" ] = bounds.max.get_e1();
	varMap[ "yMin" ] = bounds.min.get_e2();
	varMap[ "yMax" ] = bounds.max.get_e2();

	ConvertValueStringFromVarMap( valueString, varMap );
	return VSE_NONE;
}

//=================================================================================
/*virtual*/ Node::ValueStringError BoundedPointListNode::SetValueFromString( const std::string& valueString )
{
	VarMap varMap;
	if( !ConvertValueStringToVarMap( valueString, varMap ) )
		return VSE_SYNTAX;

	Bounds trialBounds = bounds;
	if( varMap.end() != varMap.find( "xMin" ) )
		trialBounds.min.set_e1( varMap[ "xMin" ] );
	if( varMap.end() != varMap.find( "xMax" ) )
		trialBounds.max.set_e1( varMap[ "xMax" ] );
	if( varMap.end() != varMap.find( "yMin" ) )
		trialBounds.min.set_e2( varMap[ "yMin" ] );
	if( varMap.end() != varMap.find( "yMax" ) )
		trialBounds.max.set_e2( varMap[ "yMax" ] );

	if( trialBounds == bounds )
		return VSE_NO_CHANGE;
	if( !trialBounds.IsValid() )
		return VSE_INVALID;
	if( !SetBounds( trialBounds ) )
		return VSE_INVALID;

	return VSE_NONE;
}

//=================================================================================
bool BoundedPointListNode::Bounds::operator==( const Bounds& bounds ) const
{
	if( min.get_e1() != bounds.min.get_e1() )
		return false;
	if( min.get_e2() != bounds.min.get_e2() )
		return false;
	if( max.get_e1() != bounds.max.get_e1() )
		return false;
	if( max.get_e2() != bounds.max.get_e2() )
		return false;
	return true;
}

//=================================================================================
/*virtual*/ Node::ValueStringError BoundedPointListNode::GetValueAsStringHelp( std::string& helpString ) const
{
	helpString = "Keys: xMin, xMax, yMin, yMax";
	return VSE_NONE;
}

//=================================================================================
bool BoundedPointListNode::SetBounded( bool bounded )
{
	if( !AllPointsInBounds( bounds ) )
		return false;

	this->bounded = bounded;
	return true;
}

//=================================================================================
bool BoundedPointListNode::IsBounded( void ) const
{
	return bounded;
}

//=================================================================================
bool BoundedPointListNode::Bounds::Contains( const c3ga::vectorE2GA& point ) const
{
	if( !IsValid() )
		return false;
	if( point.get_e1() < min.get_e1() || point.get_e1() > max.get_e1() )
		return false;
	if( point.get_e2() < min.get_e2() || point.get_e2() > max.get_e2() )
		return false;
	return true;
}

//=================================================================================
bool BoundedPointListNode::Bounds::IsValid( void ) const
{
	if( min.get_e1() > max.get_e1() )
		return false;
	if( min.get_e2() > max.get_e2() )
		return false;
	return true;
}

//=================================================================================
void BoundedPointListNode::Bounds::MinimallyExpandToIncludePoint( const c3ga::vectorE2GA& point )
{
	if( min.get_e1() > point.get_e1() )
		min.set_e1( point.get_e1() );
	if( max.get_e1() < point.get_e1() )
		max.set_e1( point.get_e1() );
	if( min.get_e2() > point.get_e2() )
		min.set_e2( point.get_e2() );
	if( max.get_e2() < point.get_e2() )
		max.set_e2( point.get_e2() );
}

//=================================================================================
bool BoundedPointListNode::FindTightestBounds( Bounds& tightestBounds ) const
{
	if( pointList->size() == 0 )
		return false;

	List::const_iterator iter = pointList->begin();
	tightestBounds.min = *iter;
	tightestBounds.max = *iter++;
	while( iter != pointList->end() )
		tightestBounds.MinimallyExpandToIncludePoint( *iter++ );

	return true;
}

//=================================================================================
bool BoundedPointListNode::SetBounds( const Bounds& givenBounds )
{
	if( !AllPointsInBounds( givenBounds ) )
		return false;

	this->bounds = givenBounds;
	return true;
}

//=================================================================================
const BoundedPointListNode::Bounds& BoundedPointListNode::GetBounds( void ) const
{
	return bounds;
}

//=================================================================================
bool BoundedPointListNode::AllPointsInBounds( const Bounds& givenBounds ) const
{
	for( List::const_iterator iter = pointList->begin(); iter != pointList->end(); iter++ )
	{
		c3ga::vectorE2GA point = *iter;
		if( !givenBounds.Contains( point ) )
			return false;
	}
	return true;
}

//=================================================================================
BoundedPointListNode::List::const_iterator BoundedPointListNode::IndexToIterator( int index ) const
{
	if( index < 0 )
		return pointList->end();

	List::const_iterator iter = pointList->begin();
	while( index-- > 0 && iter != pointList->end() )
		iter++;

	return iter;
}

//=================================================================================
int BoundedPointListNode::IndexFromIterator( List::const_iterator iter ) const
{
	if( iter == pointList->end() )
		return -1;

	int index = 0;
	while( iter != pointList->begin() )
	{
		iter--;
		index++;
	}

	return index;
}

//=================================================================================
/*virtual*/ bool BoundedPointListNode::InsertPoint( const c3ga::vectorE2GA& point, List::const_iterator* insertBeforeIter /*= 0*/, bool expandIfNeeded /*= false*/ )
{
	if( expandIfNeeded )
		bounds.MinimallyExpandToIncludePoint( point );
	else if( bounded && !bounds.Contains( point ) )
		return false;

	if( insertBeforeIter )
		pointList->insert( *insertBeforeIter, point );
	else
		pointList->push_back( point );

	return true;
}

//=================================================================================
/*virtual*/ bool BoundedPointListNode::RemovePoint( List::const_iterator& removeIter )
{
	pointList->erase( removeIter );
	return true;
}

//=================================================================================
/*virtual*/ bool BoundedPointListNode::RemoveNearestPoint( const c3ga::vectorE2GA& point, double maxSquareRadius /*= 0.0*/ )
{
	List::const_iterator removeIter = FindNearestPoint( point, maxSquareRadius );
	if( removeIter == pointList->end() )
		return false;
	return RemovePoint( removeIter );
}

//=================================================================================
/*virtual*/ bool BoundedPointListNode::SetPoint( const c3ga::vectorE2GA& point, List::const_iterator& setAtIter, bool expandIfNeeded /*= false*/ )
{
	if( expandIfNeeded )
		bounds.MinimallyExpandToIncludePoint( point );
	else if( bounded && !bounds.Contains( point ) )
		return false;

	// I found this trick on stack-overflow.
	List::iterator iter = pointList->erase( setAtIter, setAtIter );
	*iter = point;
	return true;
}

//=================================================================================
BoundedPointListNode::List::const_iterator BoundedPointListNode::FindNearestPoint( const c3ga::vectorE2GA& point, double maxSquareRadius /*= -1.0*/ ) const
{
	if( pointList->size() == 0 )
		return pointList->end();

	// Perform a linear search for the point nearest the given point.
	List::const_iterator foundIter = pointList->begin();
	c3ga::vectorE2GA nearestPoint = *foundIter;
	double nearestSquareDistance = c3ga::norm2( nearestPoint - point );
	List::const_iterator iter = foundIter;
	for( iter++; iter != pointList->end(); iter++ )
	{
		c3ga::vectorE2GA currentPoint = *iter;
		double currentSquareDistance = c3ga::norm2( currentPoint - point );
		if( currentSquareDistance < nearestSquareDistance )
		{
			nearestSquareDistance = currentSquareDistance;
			nearestPoint = currentPoint;
			foundIter = iter;
		}
	}

	if( maxSquareRadius >= 0.0 && nearestSquareDistance > maxSquareRadius )
		return pointList->end();

	return foundIter;
}

//=================================================================================
const BoundedPointListNode::List& BoundedPointListNode::GetPointList( void ) const
{
	return *pointList;
}

// BoundedPointListNode.cpp