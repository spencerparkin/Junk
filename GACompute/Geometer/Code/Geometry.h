// Geometry.h

#ifndef __Geometry_h__
#define __Geometry_h__

#include "GACompute.h"
#include "MathLibrary.h"
#include <wx/setup.h>
#include <wx/app.h>
#include <GL/gl.h>

namespace Geometer
{
	class Geometry;
}

// An instance of this class acts as glue between an operand tree at a registry-index in the stack
// and a decomposed or otherwise alternative characterization of what's represented by the expression.
class Geometer::Geometry
{
public:

	Geometry( void );
	virtual ~Geometry( void );

	virtual void Render( GLenum mode ) const = 0;
	virtual void Translate( const MathLibrary::Vector3D& translation ) = 0;
	virtual void Rotate( const MathLibrary::Vector3D& unitLengthAxis, MathLibrary::Scalar angle ) = 0;
	virtual void Scale( MathLibrary::Scalar scale ) = 0;
	
	virtual bool ComposeOperandTree( void ) const = 0;
	virtual bool DecomposeOperandTree( void ) = 0;

	// The operand tree in question here will live in the stack at
	// the registry-index that is this class instance's address.
	const GACompute::Operand* OperandTree( void ) const;
	GACompute::Operand* OperandTree( void );

	GLuint SelectionID( void ) const;

private:

	GLuint selectionID;
	static GLuint newSelectionID;
};

// TODO: When a geometry is changed, it should get pushed onto a queue.  When a geometry
//       on this queue is processed, all dependency-formulas depending on that geometry
//       should be evaluated, and then more geometries queued as appropriate.  More thought
//       is required.  Until then, just get the render interface to the geometries working.

#endif //__Geometry_h__

// Geometry.h