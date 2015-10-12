// c3ga_util.cpp

// Unlike other c3ga* files, this one is not auto-generated!

#include "c3ga.h"
#include "c3ga_util.h"
#define _USE_MATH_DEFINES
#include <math.h>

//=================================================================================
// Return a vector orthogonal to and of the same length as the given vector.
c3ga::vectorE3GA c3ga::OrthoVector( const c3ga::vectorE3GA& vector )
{
	c3ga::vectorE3GA orthoVector( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 0.0 );
	double vectorLength = c3ga::norm( vector );

	unsigned int bm;
	vector.largestBasisBlade( bm );
	switch( bm )		// I'm not sure how to interpret these "bitmaps".
	{
		case 0:
		{
			orthoVector.set( c3ga::vectorE3GA::coord_e1_e2_e3, vector.get_e2(), -vector.get_e1(), 0.0 );
			break;
		}
		case 4:
		{
			orthoVector.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, vector.get_e3(), -vector.get_e2() );
			break;
		}
		case 8:
		{
			orthoVector.set( c3ga::vectorE3GA::coord_e1_e2_e3, vector.get_e3(), 0.0, -vector.get_e1() );
			break;
		}
	}

	double orthoVectorLength = c3ga::norm( orthoVector );
	if( orthoVectorLength != 0.0 )
		orthoVector = c3ga::gp( orthoVector, vectorLength / orthoVectorLength );

	return orthoVector;
}

//=================================================================================
c3ga::rotorE3GA c3ga::RotorFromAxisAngle( const c3ga::vectorE3GA& unitAxis, double halfAngle )
{
	c3ga::rotorE3GA rotor = c3ga::exp( c3ga::gp( c3ga::gp( unitAxis, c3ga::I3 ), -halfAngle ) );
	return rotor;
}

//=================================================================================
c3ga::vectorE3GA c3ga::RotorToAxisAngle( const c3ga::rotorE3GA& unitRotor, double& halfAngle )
{
	// TODO: Write this.  Take a logarithm?  No, we can just do some trig here.
	c3ga::vectorE3GA unitAxis( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 0.0 );
	return unitAxis;
}

//=================================================================================
c3ga::vectorE3GA c3ga::CrossProduct( const c3ga::vectorE3GA& left, const c3ga::vectorE3GA& right )
{
	return c3ga::negate( c3ga::gp( c3ga::op( left, right ), c3ga::I3 ) );
}

//=================================================================================
c3ga::vectorE3GA c3ga::Lerp( const c3ga::vectorE3GA& pt0, const c3ga::vectorE3GA& pt1, double t )
{
	return c3ga::gp( pt0, 1.0 - t ) + c3ga::gp( pt1, t );
}

//=================================================================================
c3ga::vectorE2GA c3ga::Lerp( const c3ga::vectorE2GA& pt0, const c3ga::vectorE2GA& pt1, double t )
{
	return c3ga::gp( pt0, 1.0 - t ) + c3ga::gp( pt1, t );
}

// TODO: Add slerp.

//=================================================================================
c3ga::rotorE3GA c3ga::UnitRotorFromEulerAngles( double xAngle, double yAngle, double zAngle, c3ga::EulerAngleOrder eulerAngleOrder )
{
	c3ga::vectorE3GA xAxis, yAxis, zAxis;

	xAxis.set( c3ga::vectorE3GA::coord_e1_e2_e3, 1.0, 0.0, 0.0 );
	yAxis.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 1.0, 0.0 );
	zAxis.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 1.0 );

	c3ga::rotorE3GA xUnitRotor = RotorFromAxisAngle( xAxis, xAngle * 0.5 );
	c3ga::rotorE3GA yUnitRotor = RotorFromAxisAngle( yAxis, yAngle * 0.5 );
	c3ga::rotorE3GA zUnitRotor = RotorFromAxisAngle( zAxis, zAngle * 0.5 );

	c3ga::rotorE3GA unitRotor;
	unitRotor.set( c3ga::rotorE3GA::coord_scalar_e1e2_e2e3_e3e1, 1.0, 0.0, 0.0, 0.0 );

	switch( eulerAngleOrder )
	{
		case c3ga::EULER_XYZ:
		{
			// Here, z is applied first, then y, then x, which is the reverse order of XYZ.
			// In terms of Euler rotations, x is applied first, then y, then z.
			unitRotor = xUnitRotor * yUnitRotor * zUnitRotor;
			break;
		}
		case c3ga::EULER_XZY:
		{
			unitRotor = xUnitRotor * zUnitRotor * yUnitRotor;
			break;
		}
		case c3ga::EULER_YXZ:
		{
			unitRotor = yUnitRotor * xUnitRotor * zUnitRotor;
			break;
		}
		case c3ga::EULER_YZX:
		{
			unitRotor = yUnitRotor * zUnitRotor * xUnitRotor;
			break;
		}
		case c3ga::EULER_ZXY:
		{
			unitRotor = zUnitRotor * xUnitRotor * yUnitRotor;
			break;
		}
		case c3ga::EULER_ZYX:
		{
			unitRotor = zUnitRotor * yUnitRotor * xUnitRotor;
			break;
		}
	}

	return unitRotor;
}

//=================================================================================
bool c3ga::UnitRotorToEulerAngles( const c3ga::rotorE3GA& unitRotor, double& xAngle, double& yAngle, double& zAngle, c3ga::EulerAngleOrder eulerAngleOrder )
{
	c3ga::vectorE3GA xAxis, yAxis, zAxis;

	xAxis.set( c3ga::vectorE3GA::coord_e1_e2_e3, 1.0, 0.0, 0.0 );
	yAxis.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 1.0, 0.0 );
	zAxis.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 1.0 );

	c3ga::vectorE3GA xAxisP, yAxisP, zAxisP;

	xAxisP = c3ga::applyUnitVersor( unitRotor, xAxis );
	yAxisP = c3ga::applyUnitVersor( unitRotor, yAxis );
	zAxisP = c3ga::applyUnitVersor( unitRotor, zAxis );

	c3ga::vectorE3GA* axis[3] = { 0, 0, 0 };
	c3ga::vectorE3GA* axisP[3] = { 0, 0, 0 };
	double* angle[3] = { 0, 0, 0 };

	double sign = 1.0;

	// Setup the problem.
	switch( eulerAngleOrder )
	{
		case c3ga::EULER_XYZ:
		{
			axis[0] = &xAxis;
			axis[1] = &yAxis;
			axis[2] = &zAxis;
			axisP[0] = &xAxisP;
			axisP[1] = &yAxisP;
			axisP[2] = &zAxisP;
			angle[0] = &xAngle;
			angle[1] = &yAngle;
			angle[2] = &zAngle;
			sign = -1.0;
			break;
		}
		case c3ga::EULER_XZY:
		{
			axis[0] = &xAxis;
			axis[1] = &zAxis;
			axis[2] = &yAxis;
			axisP[0] = &xAxisP;
			axisP[1] = &zAxisP;
			axisP[2] = &yAxisP;
			angle[0] = &xAngle;
			angle[1] = &zAngle;
			angle[2] = &yAngle;
			break;
		}
		case c3ga::EULER_YXZ:
		{
			axis[0] = &yAxis;
			axis[1] = &xAxis;
			axis[2] = &zAxis;
			axisP[0] = &yAxisP;
			axisP[1] = &xAxisP;
			axisP[2] = &zAxisP;
			angle[0] = &yAngle;
			angle[1] = &xAngle;
			angle[2] = &zAngle;
			break;
		}
		case c3ga::EULER_YZX:
		{
			axis[0] = &yAxis;
			axis[1] = &zAxis;
			axis[2] = &xAxis;
			axisP[0] = &yAxisP;
			axisP[1] = &zAxisP;
			axisP[2] = &xAxisP;
			angle[0] = &yAngle;
			angle[1] = &zAngle;
			angle[2] = &xAngle;
			sign = -1.0;
			break;
		}
		case c3ga::EULER_ZXY:
		{
			axis[0] = &zAxis;
			axis[1] = &xAxis;
			axis[2] = &yAxis;
			axisP[0] = &zAxisP;
			axisP[1] = &xAxisP;
			axisP[2] = &yAxisP;
			angle[0] = &zAngle;
			angle[1] = &xAngle;
			angle[2] = &yAngle;
			sign = -1.0;
			break;
		}
		case c3ga::EULER_ZYX:
		{
			axis[0] = &zAxis;
			axis[1] = &yAxis;
			axis[2] = &xAxis;
			axisP[0] = &zAxisP;
			axisP[1] = &yAxisP;
			axisP[2] = &xAxisP;
			angle[0] = &zAngle;
			angle[1] = &yAngle;
			angle[2] = &xAngle;
			break;
		}
		default:
		{
			return false;
		}
	}

	// Deduce the first euler angle.
	double a1_dot_a2p = c3ga::lc( *axis[1], *axisP[2] );
	double a2_dot_a2p = c3ga::lc( *axis[2], *axisP[2] );
	if( a1_dot_a2p == 0.0 )
		*angle[0] = 0.0;			// Our goal of getting axis[1] orthogonal to axisP[2] is already done.
	else if( a2_dot_a2p == 0.0 )
		*angle[0] = M_PI * 0.5;		// We need to rotate by 90 degrees to get axis[1] orthogonal to axisP[2].
	else
		*angle[0] = atan( sign * a1_dot_a2p / a2_dot_a2p );		// Here we solve for the needed angle of rotation.

	// Keep the angle in [0,2pi].
	if( *angle[0] < 0.0 )
		*angle[0] += 2.0 * M_PI;

	// Apply the first euler angle.
	c3ga::rotorE3GA rotor = c3ga::RotorFromAxisAngle( *axis[0], *angle[0] * 0.5 );
	*axis[1] = c3ga::unit( c3ga::applyUnitVersor( rotor, *axis[1] ) );	// Normalize to kill round-off error.
	*axis[2] = c3ga::unit( c3ga::applyUnitVersor( rotor, *axis[2] ) );	// Normalize to kill round-off error.

	// Double check our work so far.
	double epsilon = 1e-3;
	a1_dot_a2p = c3ga::lc( *axis[1], *axisP[2] );	// This should now be zero.
	if( fabs( a1_dot_a2p ) >= epsilon )
		return false;

	// Deduce the second euler angle.
	a2_dot_a2p = c3ga::lc( *axis[2], *axisP[2] );
	*angle[1] = acos( a2_dot_a2p );		// We solve for the angle that rotates axis[2] into axisP[2].
	c3ga::trivectorE3GA trivector = *axis[2] ^ *axisP[2] ^ *axis[1];
	if( trivector.get_e1_e2_e3() < 0.0 )
		*angle[1] *= -1.0;

	// Keep the angle in [0,2pi].
	if( *angle[1] < 0.0 )
		*angle[1] += 2.0 * M_PI;

	// Apply the second euler angle.
	rotor = c3ga::RotorFromAxisAngle( *axis[1], *angle[1] * 0.5 );
	*axis[0] = c3ga::unit( c3ga::applyUnitVersor( rotor, *axis[0] ) );	// Normalize to kill round-off error.
	*axis[2] = c3ga::unit( c3ga::applyUnitVersor( rotor, *axis[2] ) );	// Normalize to kill round-off error.

	// Double check our work so far.
	double length = c3ga::norm( *axis[2] - *axisP[2] );
	if( length >= epsilon )
		return false;

	// Deduce the third euler angle.
	double a1_dot_a1p = c3ga::lc( *axis[1], *axisP[1] );
	*angle[2] = acos( a1_dot_a1p );
	trivector = *axis[1] ^ *axisP[1] ^ *axis[2];
	if( trivector.get_e1_e2_e3() < 0.0 )
		*angle[2] *= -1.0;

	// Keep the angle in [0,2pi].
	if( *angle[2] < 0.0 )
		*angle[2] += 2.0 * M_PI;

	// Apply the third euler angle.
	rotor = c3ga::RotorFromAxisAngle( *axis[2], *angle[2] * 0.5 );
	*axis[0] = c3ga::applyUnitVersor( rotor, *axis[0] );
	*axis[1] = c3ga::applyUnitVersor( rotor, *axis[1] );

	// Double check our work.
	length = c3ga::norm( *axis[0] - *axisP[0] );
	if( length >= epsilon )
		return false;
	length = c3ga::norm( *axis[1] - *axisP[1] );
	if( length >= epsilon )
		return false;

	// We have succeeded!
	return true;
}

// c3ga_util.cpp