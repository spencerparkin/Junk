// Sphere.cpp

#include "Sphere.h"

using namespace Geometer;

Sphere::Sphere( void )
{
	radius = 1.0;
	type = REAL;
	interpretation = CONFORMAL_SPHERE_DUAL;
}

/*virtual*/ Sphere::~Sphere( void )
{
}

/*virtual*/ void Sphere::Render( GLenum mode ) const
{
}

/*virtual*/ void Sphere::Translate( const MathLibrary::Vector3D& translation )
{
	center += translation;
}

/*virtual*/ void Sphere::Rotate( const MathLibrary::Vector3D& unitLengthAxis, MathLibrary::Scalar angle )
{
	// Rotating a sphere about its center is trivial.  We do nothing!
}

/*virtual*/ void Sphere::Scale( MathLibrary::Scalar scale )
{
	// TODO: May have a problem with degenerate spheres here.
	radius *= scale;
}

/*virtual*/ bool Sphere::ComposeOperandTree( void ) const
{
	// TODO: Use formatted string and Lua's do-string function here?
	return false;
}

/*virtual*/ bool Sphere::DecomposeOperandTree( void )
{
	return false;
}

// Sphere.cpp