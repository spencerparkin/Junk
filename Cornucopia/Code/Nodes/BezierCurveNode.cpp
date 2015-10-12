// BezierCurveNode.cpp

#include "../Cornucopia.h"

using namespace Cornucopia;

IMPL_CORNUCOPIA_CLASSINFO1( BezierCurveNode, BoundedPointListNode );

//=================================================================================
BezierCurveNode::BezierCurveNode( void )
{
	constraintFlags = 0;
	pointArrayCacheValid = false;
	pointArrayCache = new std::vector< c3ga::vectorE2GA >();
}

//=================================================================================
/*virtual*/ BezierCurveNode::~BezierCurveNode( void )
{
	delete pointArrayCache;
}

//=================================================================================
/*virtual*/ bool BezierCurveNode::ReadFromTable( lua_State* L, Context& context )
{
	if( !BoundedPointListNode::ReadFromTable( L, context ) )
		return false;

	//...

	return true;
}

//=================================================================================
/*virtual*/ bool BezierCurveNode::WriteToTable( lua_State* L, Context& context ) const
{
	if( !BoundedPointListNode::WriteToTable( L, context ) )
		return false;

	//...

	return true;
}

//=================================================================================
/*virtual*/ bool BezierCurveNode::Copy( const Node* node, Context& context, const CopyParameters& copyParameters )
{
	if( !BoundedPointListNode::Copy( node, context, copyParameters ) )
		return false;

	//...

	return true;
}

//=================================================================================
/*virtual*/ std::string BezierCurveNode::GetValueToDisplayString( void ) const
{
	std::string displayString = BoundedPointListNode::GetValueToDisplayString();

	//...

	return displayString;
}

//=================================================================================
/*virtual*/ Node::ValueStringError BezierCurveNode::GetValueToString( std::string& valueString ) const
{
	ValueStringError vse = BoundedPointListNode::GetValueToString( valueString );
	if( vse == VSE_NONE )
	{
		//...
	}

	return vse;
}

//=================================================================================
/*virtual*/ Node::ValueStringError BezierCurveNode::SetValueFromString( const std::string& valueString )
{
	ValueStringError vse = BoundedPointListNode::SetValueFromString( valueString );
	if( vse == VSE_NONE || vse == VSE_NO_CHANGE )
	{
		//...
	}

	return vse;
}

//=================================================================================
/*virtual*/ Node::ValueStringError BezierCurveNode::GetValueAsStringHelp( std::string& helpString ) const
{
	ValueStringError vse = BoundedPointListNode::GetValueAsStringHelp( helpString );
	if( vse == VSE_NONE )
	{
		//...
	}

	return vse;
}

//=================================================================================
/*virtual*/ bool BezierCurveNode::InsertPoint( const c3ga::vectorE2GA& point, List::const_iterator* insertBeforeIter /*= 0*/, bool expandIfNeeded /*= false*/ )
{
	if( !BoundedPointListNode::InsertPoint( point, insertBeforeIter, expandIfNeeded ) )
		return false;

	pointArrayCacheValid = false;
	return true;
}

//=================================================================================
/*virtual*/ bool BezierCurveNode::RemovePoint( List::const_iterator& removeIter )
{
	if( !BoundedPointListNode::RemovePoint( removeIter ) )
		return false;

	pointArrayCacheValid = false;
	return true;
}

//=================================================================================
/*virtual*/ bool BezierCurveNode::RemoveNearestPoint( const c3ga::vectorE2GA& point, double maxSquareRadius /*= 0.0*/ )
{
	if( !BoundedPointListNode::RemoveNearestPoint( point, maxSquareRadius ) )
		return false;

	pointArrayCacheValid = false;
	return true;
}

//=================================================================================
/*virtual*/ bool BezierCurveNode::SetPoint( const c3ga::vectorE2GA& point, List::const_iterator& setAtIter, bool expandIfNeeded /*= false*/ )
{
	if( !BoundedPointListNode::SetPoint( point, setAtIter, expandIfNeeded ) )
		return false;
	
	pointArrayCacheValid = false;
	return true;
}

//=================================================================================
bool BezierCurveNode::IncrementDegree( void )
{
	if( pointList->size() == 0 )
		return false;

	RepopulatePointArrayCacheIfNeeded();
	pointList->clear();

	// Interestingly, this not only preserves the curve shape, but also the parameterization of the curve.
	for( int index = 0; index < ( signed )pointArrayCache->size() + 1; index++ )
	{
		double t = double( index ) / double( pointArrayCache->size() );
		c3ga::vectorE2GA p0( c3ga::vectorE2GA::coord_e1_e2, 0.0, 0.0 );
		if( index > 0 )
			p0 = ( *pointArrayCache )[ index - 1 ];
		c3ga::vectorE2GA p1( c3ga::vectorE2GA::coord_e1_e2, 0.0, 0.0 );
		if( index < ( signed )pointArrayCache->size() )
			p1 = ( *pointArrayCache )[ index ];
		c3ga::vectorE2GA p = c3ga::gp( p0, t ) + c3ga::gp( p1, 1.0 - t );
		pointList->push_back( p );
	}

	pointArrayCacheValid = false;
	return true;
}

//=================================================================================
bool BezierCurveNode::DecrementDegree( void )
{
	if( pointList->size() == 0 )
		return false;

	if( pointList->size() == 1 )
		pointList->clear();
	else if( pointList->size() == 2 )
	{
		List::iterator first = pointList->begin();
		List::iterator last = --pointList->end();
		c3ga::vectorE2GA p0 = *first;
		c3ga::vectorE2GA p1 = *last;
		c3ga::vectorE2GA p = c3ga::gp( p0, 0.5 ) + c3ga::gp( p1, 0.5 );
		*first = p;
		pointList->erase( last );
	}
	else
	{
		RepopulatePointArrayCacheIfNeeded();
		pointList->clear();

		// This will only be an approximation of the original curve.
		for( int index = 0; index < ( signed )pointArrayCache->size() - 1; index++ )
		{
			double t = double( index ) / double( pointArrayCache->size() - 2 );
			c3ga::vectorE2GA p0 = ( *pointArrayCache )[ index ];
			c3ga::vectorE2GA p1 = ( *pointArrayCache )[ index + 1 ];
			c3ga::vectorE2GA p = c3ga::gp( p0, 1.0 - t ) + c3ga::gp( p1, t );
			pointList->push_back( p );
		}
	}

	pointArrayCacheValid = false;
	return true;
}

//=================================================================================
int BezierCurveNode::GetDegree( void ) const
{
	return signed( pointList->size() ) - 1;
}

//=================================================================================
bool BezierCurveNode::Evaluate( double t, c3ga::vectorE2GA& point ) const
{
	if( pointList->size() == 0 )
		return false;
	
	RepopulatePointArrayCacheIfNeeded();

	point.m_e1 = DeCasteljuaEvaluate( 0, pointList->size() - 1, t, &BezierCurveNode::CoefFuncForX );
	point.m_e2 = DeCasteljuaEvaluate( 0, pointList->size() - 1, t, &BezierCurveNode::CoefFuncForY );
	return true;
}

//=================================================================================
bool BezierCurveNode::EvaluateDerivative( double t, c3ga::vectorE2GA& point ) const
{
	if( pointList->size() == 0 )
		return false;

	RepopulatePointArrayCacheIfNeeded();
	
	point.m_e2 = DeCasteljuaEvaluateDerivative( 0, pointList->size() - 1, t, &BezierCurveNode::CoefFuncForX );
	point.m_e2 = DeCasteljuaEvaluateDerivative( 0, pointList->size() - 1, t, &BezierCurveNode::CoefFuncForY );
	return true;
}

//=================================================================================
double BezierCurveNode::CoefFuncForX( int index ) const
{
	return ( *pointArrayCache )[ index ].get_e1();
}

//=================================================================================
double BezierCurveNode::CoefFuncForY( int index ) const
{
	return ( *pointArrayCache )[ index ].get_e2();
}

//=================================================================================
// Non-recursive versions of DeCaseljua's algorithm are a very fast way to
// evaluate Bezier curves.  Being recursive, this one is not so fast.
double BezierCurveNode::DeCasteljuaEvaluate( int a, int b, double t, CoeficientFunction coefFunc ) const
{
	double p0, p1, p2, p3;

	// Special case this for speed and accuracy.
	if( b - a == 3 )
	{
		p0 = ( this->*coefFunc )( a + 0 );
		p1 = ( this->*coefFunc )( a + 1 );
		p2 = ( this->*coefFunc )( a + 2 );
		p3 = ( this->*coefFunc )( a + 3 );

		double t2 = t * t;
		double t3 = t2 * t;
		double omt = 1.0 - t;
		double omt2 = omt * omt;
		double omt3 = omt2 * omt;

		return omt3 * p0 +
				3.0 * omt2 * t * p1 +
				3.0 * omt * t2 * p2 +
				t3 * p3;
	}

	if( a == b )
		return ( this->*coefFunc )( a );

	p0 = DeCasteljuaEvaluate( a, b - 1, t, coefFunc );
	p1 = DeCasteljuaEvaluate( a + 1, b, t, coefFunc );
	return ( 1.0 - t ) * p0 + t * p1;
}

//=================================================================================
// Is there a non-recursive way to do this?
double BezierCurveNode::DeCasteljuaEvaluateDerivative( int a, int b, double t, CoeficientFunction coefFunc ) const
{
	double p0, p1, p2, p3;

	// Special case this for speed and accuracy.
	if( b - a == 3 )
	{
		p0 = ( this->*coefFunc )( a + 0 );
		p1 = ( this->*coefFunc )( a + 1 );
		p2 = ( this->*coefFunc )( a + 2 );
		p3 = ( this->*coefFunc )( a + 3 );

		double t2 = t * t;
		double omt = 1.0 - t;
		double omt2 = omt * omt;

		return -3.0 * omt2 * p0 +
				3.0 * ( -2.0 * omt * t + omt2 ) * p1 +
				3.0 * ( -t2 + 2.0 * omt * t ) * p2 +
				3.0 * t2 * p3;
	}

	if( a == b )
		return 0.0;

	p0 = DeCasteljuaEvaluate( a, b - 1, t, coefFunc );
	p1 = DeCasteljuaEvaluateDerivative( a, b - 1, t, coefFunc );
	p2 = DeCasteljuaEvaluate( a + 1, b, t, coefFunc );
	p3 = DeCasteljuaEvaluateDerivative( a + 1, b, t, coefFunc );
	return -p0 + ( 1.0 - t ) * p1 + p2 + t * p3;
}

//=================================================================================
bool BezierCurveNode::Evaluate( double x, double& y, int newtonIters ) const
{
	if( pointList->size() == 0 )
		return false;

	RepopulatePointArrayCacheIfNeeded();

	double x0 = CoefFuncForX(0);
	double x1 = CoefFuncForX( pointList->size() - 1 );
	double t = ( x - x0 ) / ( x1 - x0 );

	double lastError, error, deriv;
	for( int iter = 0; iter < newtonIters; iter++ )
	{
		error = x - DeCasteljuaEvaluate( 0, pointList->size() - 1, t, &BezierCurveNode::CoefFuncForX );
		if( iter > 0 && fabs( error ) > fabs( lastError ) )
			break;
		deriv = DeCasteljuaEvaluateDerivative( 0, pointList->size() - 1, t, &BezierCurveNode::CoefFuncForX );
		t += error / deriv;
		lastError = error;
	}

	y = DeCasteljuaEvaluate( 0, pointList->size() - 1, t, &BezierCurveNode::CoefFuncForY );
	return true;
}

//=================================================================================
int BezierCurveNode::GetConstraintFlags( void ) const
{
	return constraintFlags;
}

//=================================================================================
void BezierCurveNode::SetConstraintFlags( int constraintFlags )
{
	this->constraintFlags = constraintFlags;
	ApplyConstraints();
}

//=================================================================================
void BezierCurveNode::ApplyConstraints( void )
{
	//...
}

//=================================================================================
// The base class does not maintain its point-list as an array.  (Perhaps it
// should have done so.)  But since it doesn't, here we convert the list to
// an array for fast look-ups during curve evaluation procedures.
void BezierCurveNode::RepopulatePointArrayCacheIfNeeded( void ) const
{
	if( !pointArrayCacheValid )
	{
		pointArrayCache->resize( pointList->size() );

		for( int index = 0; index < ( signed )pointList->size(); index++ )
		{
			List::const_iterator iter = IndexToIterator( index );
			( *pointArrayCache )[ index ] = *iter;
		}

		pointArrayCacheValid = true;
	}
}

// BezierCurveNode.cpp