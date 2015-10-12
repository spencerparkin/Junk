// Sphere.h

#ifndef __Sphere_h__
#define __Sphere_h__

#include "Geometry.h"

namespace Geometer
{
	class Sphere;
}

// TODO: We'll need a function (that works for any derivative) that can create a new sphere
//       and put it in the application's map and bind it to an existing operand tree.  The
//       function takes the operand tree and the desired way of interpreting it.
class Geometer::Sphere : public Geometer::Geometry
{
public:

	Sphere( void );
	virtual ~Sphere( void );

	virtual void Render( GLenum mode ) const override;
	virtual void Translate( const MathLibrary::Vector3D& translation ) override;
	virtual void Rotate( const MathLibrary::Vector3D& unitLengthAxis, MathLibrary::Scalar angle ) override;
	virtual void Scale( MathLibrary::Scalar scale ) override;
	
	virtual bool ComposeOperandTree( void ) const override;
	virtual bool DecomposeOperandTree( void ) override;

	enum Interpretation
	{
		CONFORMAL_SPHERE_DIRECT,
		CONFORMAL_SPHERE_DUAL,
	};

private:

	MathLibrary::Vector3D center;
	MathLibrary::Scalar radius;
	
	enum Type
	{
		REAL,
		IMAGINARY,
	};

	Type type;

	Interpretation interpretation;
};

#endif //__Sphere_h__

// Sphere.h