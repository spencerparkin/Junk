// BinaryGate.h

// This module supports gates with 2 inputs
// and 1 output only.  Put together, however,
// they can represent gates that can take
// multiple inputs and give multiple outputs.
// These are all just nodes in a dependancy graph.
// So combinatorial circuits are fully supported.
// However, circular dependencies will require
// some poking of values into them before they'll work.

#ifndef __BinaryGate_h__
#define __BinaryGate_h__

#include <GL/glut.h>
#include <math.h>

#define PI			3.1415926536

enum
{
	ZERO,			// Off, false, no, etc...
	ONE,			// On, true, yes, etc...
	NOTHING,		// This is simply an indeterminant state.  We are still dealing with binary digital circuits here.
};

class BinaryGate
{
	public:

	BinaryGate(GLfloat x, GLfloat y, GLfloat angle);
	~BinaryGate();

	// All gates will display their
	// schematic symbol and the inputs
	// and outputs they have.
	virtual void Draw(GLenum mode) = 0;

	// All gates perform some type
	// of binary operation on their
	// pair of operands.
	virtual void Operate(void) = 0;

	// This gives the user a chance to
	// "tweak" this gate.  What this
	// actually does is gate dependent.
	virtual void Tweak(void) = 0;

	// The gate can't do anything until
	// it has data it can operate upon.
	int InputIsReady(void);

	// The gate stuff...
	BinaryGate *input0, *input1;
	int output;	
	GLfloat x, y;
	GLfloat angle;
};

// The following routine is useful for drawing
// parts of the gate diagrams.
void DrawArc(GLfloat x, GLfloat y, GLfloat angle, GLfloat radius, GLfloat arc_radians, int num_steps);

// This routine can be used to draw orthogonal connection
// lines from one gate's output to another gate's input.
void DrawConnection(GLfloat x0, GLfloat y0, GLfloat x1, GLfloat y1);
void DrawConnection(BinaryGate *gate0, BinaryGate *gate1, GLfloat x0, GLfloat y0, GLfloat x1, GLfloat y1);

// This routine will draw a "1", a "0", or
// nothing if the state of the line is indeterminant.
void DrawBit(GLfloat x, GLfloat y, GLfloat scale, int bit);

// This routine can be used to draw a collection of
// gates that make up an entire circuit.
void DrawCircuit(BinaryGate **gate, int num_gates);

// The following routine is used to simulate
// the functioning of a collection of gates,
// assumed to be connected to one another.
void IterateCircuit(BinaryGate **gate, int num_gates);

// This routine can be used to reset the state
// of all gates in the given circuit to the
// indeterminent state.
void WipeCircuit(BinaryGate **gate, int num_gates);

// This routine can be used to determine which
// gate the user has clicked on.
BinaryGate *SelectGate(BinaryGate **gate, int num_gates, GLfloat mouse_x, GLfloat mouse_y);

#endif __BinaryGate_h__

// endof BinaryGate.h